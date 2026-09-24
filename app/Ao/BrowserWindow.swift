import AppKit
import CAo

// isVertical is a vertical divider, so side-by-side panes. The outer split stacks.
@MainActor
final class BrowserWindow: NSObject, NSTableViewDataSource, NSTableViewDelegate {
  let model = BrowserModel()
  let window: NSWindow

  private let categoryTable = NSTableView()
  private let classTable = NSTableView()
  private let protocolTable = NSTableView()
  private let selectorTable = NSTableView()
  private let sourceView: NSTextView
  private let errorField: NSTextField
  private let sideControl: NSSegmentedControl

  private var categoryName = "Kernel"
  private var selectedClass = "Object"
  private var meta = false
  // nil protocol: the pane is the class definition. A protocol with no selector: a new method.
  private var protocolName: String? = "native"
  private var selectorName: String? = "printString"
  private var applying = false
  private var showingHierarchy = false
  private var hierarchyNames: [String] = []
  // The text the pane got from the model; the pane differs from it after an unaccepted edit.
  private var shownSource = ""
  // A discard question is waiting for its answer.
  private var confirming = false

  // Asked before a selection change would replace an unaccepted edit. The callback gets true to
  // discard the edit and change the selection, false to keep both. Tests replace it.
  typealias DiscardConfirmation = @MainActor (NSWindow, @escaping @MainActor (Bool) -> Void) -> Void
  var confirmDiscard: DiscardConfirmation = BrowserWindow.askToDiscard

  var hasUnacceptedChanges: Bool {
    sourceView.isEditable && sourceView.string != shownSource
  }

  var title: String {
    window.title
  }

  var sourceText: String {
    sourceView.string
  }

  var errorText: String {
    errorField.stringValue
  }

  var paneAccessibilityLabels: [String?] {
    [
      categoryTable.accessibilityLabel(),
      classTable.accessibilityLabel(),
      protocolTable.accessibilityLabel(),
      selectorTable.accessibilityLabel(),
      sourceView.accessibilityLabel(),
      errorField.accessibilityLabel(),
    ]
  }

  override init() {
    _ = NSApplication.shared
    let frame = NSRect(x: 220, y: 120, width: 880, height: 560)
    // NSSplitView keeps the current size ratio, so a zero frame stays collapsed.
    let band = NSRect(x: 0, y: 0, width: frame.width, height: frame.height / 3)
    let column = NSRect(x: 0, y: 0, width: frame.width / 2, height: band.height)
    let window = NSWindow(
      contentRect: frame,
      styleMask: [.titled, .closable, .miniaturizable, .resizable],
      backing: .buffered,
      defer: false
    )
    window.title = "System Browser"
    window.isReleasedWhenClosed = false
    window.isRestorable = false

    let source = BrowserWindow.makeSource(frame: band)
    let errorField = BrowserWindow.makeErrorField()
    let sideControl = NSSegmentedControl(
      labels: ["instance", "class"],
      trackingMode: .selectOne,
      target: nil,
      action: nil
    )
    sideControl.selectedSegment = 0

    self.window = window
    self.sourceView = source.text
    self.errorField = errorField
    self.sideControl = sideControl
    super.init()

    sideControl.target = self
    sideControl.action = #selector(sideChanged(_:))
    configure(categoryTable)
    configure(classTable)
    configure(protocolTable)
    configure(selectorTable)
    categoryTable.setAccessibilityLabel("Class categories")
    classTable.setAccessibilityLabel("Classes")
    protocolTable.setAccessibilityLabel("Protocols")
    selectorTable.setAccessibilityLabel("Selectors")
    sourceView.setAccessibilityLabel("Source")
    errorField.setAccessibilityLabel("Error")

    let categoryScroll = scroll(for: categoryTable, frame: column)
    let classes = classPane(frame: column)
    let top = NSSplitView(frame: band)
    top.isVertical = true
    top.dividerStyle = .thin
    top.addArrangedSubview(categoryScroll)
    top.addArrangedSubview(classes)
    categoryScroll.frame = column
    classes.frame = column

    let protocolScroll = scroll(for: protocolTable, frame: column)
    let selectorScroll = scroll(for: selectorTable, frame: column)
    let middle = NSSplitView(frame: band)
    middle.isVertical = true
    middle.dividerStyle = .thin
    middle.addArrangedSubview(protocolScroll)
    middle.addArrangedSubview(selectorScroll)
    protocolScroll.frame = column
    selectorScroll.frame = column

    let outer = NSSplitView(frame: NSRect(origin: .zero, size: frame.size))
    outer.isVertical = false
    outer.dividerStyle = .thin
    outer.autoresizingMask = [.width, .height]
    let holder = NSView(frame: band)
    holder.autoresizingMask = [.width, .height]
    let errorHeight: CGFloat = 22
    source.scroll.frame = NSRect(
      x: 0,
      y: errorHeight,
      width: band.width,
      height: max(band.height - errorHeight, 1)
    )
    source.scroll.autoresizingMask = [.width, .height]
    errorField.frame = NSRect(x: 6, y: 3, width: max(band.width - 12, 1), height: 16)
    errorField.autoresizingMask = [.width]
    holder.addSubview(source.scroll)
    holder.addSubview(errorField)
    outer.addArrangedSubview(top)
    outer.addArrangedSubview(middle)
    outer.addArrangedSubview(holder)
    top.frame = band
    middle.frame = band
    holder.frame = band
    window.contentView = outer

    showInitialSelection()
    window.makeKeyAndOrderFront(nil)
  }

  func numberOfRows(in tableView: NSTableView) -> Int {
    rows(of: tableView).count
  }

  func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
    value(at: row, in: rows(of: tableView))
  }

  func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {
    guard let text = value(at: row, in: rows(of: tableView)) else {
      return nil
    }
    let identifier = NSUserInterfaceItemIdentifier("name")
    let cell = tableView.makeView(withIdentifier: identifier, owner: self) as? NSTextField
      ?? NSTextField(labelWithString: "")
    cell.identifier = identifier
    cell.stringValue = text
    cell.isEditable = false
    cell.isBordered = false
    cell.drawsBackground = false
    cell.lineBreakMode = .byTruncatingTail
    return cell
  }

  func tableViewSelectionDidChange(_ notification: Notification) {
    guard !applying, let table = notification.object as? NSTableView else {
      return
    }
    let row = table.selectedRow
    // A new category, class or side shows the class definition: no protocol, no selector.
    if table === categoryTable {
      guard let name = value(at: row, in: model.categories) else {
        showSelection()
        return
      }
      changeSelection {
        self.categoryName = name
        self.protocolName = nil
        self.selectorName = nil
      }
    } else if table === classTable {
      let name = value(at: row, in: model.classes)
      changeSelection {
        if let name {
          self.selectedClass = name
        }
        self.protocolName = nil
        self.selectorName = nil
      }
    } else if table === protocolTable {
      let name = value(at: row, in: model.protocols)
      changeSelection {
        self.protocolName = name
        self.selectorName = nil
      }
    } else if table === selectorTable {
      let name = value(at: row, in: model.selectors)
      changeSelection {
        self.selectorName = name
      }
    }
  }

  func ownsWindow(_ candidate: NSWindow?) -> Bool {
    candidate === window
  }

  func replaceSource(_ value: String) {
    sourceView.string = value
  }

  func accept() {
    // SPEC §3.10: a method without source shows a placeholder; accepting it would replace the
    // method, so the read-only pane never goes to the runtime.
    guard !model.sourceIsPlaceholder else {
      errorField.stringValue = "source not available"
      return
    }
    let source = sourceView.string
    let method = acceptsMethod
    let outcome = submit(source, method: method)
    guard outcome.status == Int32(AO_OK) else {
      errorField.stringValue = failureText(status: outcome.status, message: outcome.message)
      return
    }
    errorField.stringValue = ""
    guard method else {
      showDefinedClass(from: source)
      return
    }
    // The accepted method is a CompiledMethod: show it in its protocol with its own source.
    protocolName = BrowserModel.newMethodProtocol
    selectorName = nil
    publish()
    selectorName = model.selector(withSource: source)
    publish()
  }

  // The accepted definition's class, in the category the runtime now lists for it. The rows the
  // pane came from may be another class, or a category the class has just left.
  private func showDefinedClass(from source: String) {
    if let name = BrowserWindow.definedClassName(in: source),
       let category = model.category(ofClass: name) {
      categoryName = category
      selectedClass = name
    }
    publish()
  }

  // The argument of the first subclass: keyword (#Name, #'Name', 'Name' or Name), as the chunk
  // parser takes it. A subclass: in a comment or a literal is not that keyword.
  private static func definedClassName(in source: String) -> String? {
    var scanner = DefinitionScanner(source)
    while let token = scanner.next() {
      guard token == .keyword("subclass:") else {
        continue
      }
      guard case .value(let name)? = scanner.next() else {
        return nil
      }
      return name
    }
    return nil
  }

  func showHierarchy() {
    // The question already up decides alone.
    guard !confirming else {
      return
    }
    guard hasUnacceptedChanges else {
      toggleHierarchy()
      return
    }
    confirmBeforeDiscarding { proceed in
      if proceed {
        self.toggleHierarchy()
      }
    }
  }

  private func toggleHierarchy() {
    if showingHierarchy {
      showingHierarchy = false
      hierarchyNames = []
      publish()
      return
    }
    hierarchyNames = model.hierarchyNames(className: selectedClass, meta: meta)
    showingHierarchy = true
    model.applyHierarchyList(hierarchyNames, selecting: selectedClass)
    reloadLists()
  }

  func noteImageLoaded() {
    showingHierarchy = false
    hierarchyNames = []
    publish()
  }

  @objc private func sideChanged(_ sender: NSSegmentedControl) {
    guard !applying else {
      return
    }
    let classSide = sender.selectedSegment == 1
    changeSelection {
      self.meta = classSide
      self.protocolName = nil
      self.selectorName = nil
    }
  }

  // An unaccepted edit asks first: discarding applies the change, cancelling puts the rows and
  // the side switch back and keeps the edit. A change while the question is up is not asked
  // again; its rows and switch go back at once.
  private func changeSelection(_ change: @escaping () -> Void) {
    let apply = {
      change()
      self.publish()
    }
    guard !confirming else {
      showSelection()
      return
    }
    guard hasUnacceptedChanges else {
      apply()
      return
    }
    confirmBeforeDiscarding { proceed in
      if proceed {
        apply()
      } else {
        self.showSelection()
      }
    }
  }

  // One question at a time. The answer meets the pane as it is when it comes: with no unaccepted
  // edit left, Cancel has nothing to keep, so the change goes ahead.
  private func confirmBeforeDiscarding(_ proceed: @escaping @MainActor (Bool) -> Void) {
    confirming = true
    confirmDiscard(window) { discard in
      self.confirming = false
      proceed(discard || !self.hasUnacceptedChanges)
    }
  }

  private func showInitialSelection() {
    _ = model.boot()
    model.refresh()
    if model.categories.contains("Kernel") {
      categoryName = "Kernel"
    } else if let first = model.categories.first {
      categoryName = first
    }
    selectedClass = "Object"
    meta = false
    protocolName = "native"
    selectorName = "printString"
    publish()
  }

  private func publish() {
    let keepClass = selectedClass
    model.select(
      category: categoryName,
      className: selectedClass,
      meta: meta,
      protocol: protocolName,
      selector: selectorName
    )
    if showingHierarchy {
      model.applyHierarchyList(hierarchyNames, selecting: keepClass)
    }
    selectedClass = model.selectedClass ?? ""
    protocolName = model.selectedProtocol
    selectorName = model.selectedSelector
    reloadLists()
  }

  private func reloadLists() {
    applying = true
    categoryTable.reloadData()
    classTable.reloadData()
    protocolTable.reloadData()
    selectorTable.reloadData()
    applying = false
    showSelection()
    sourceView.string = model.source
    shownSource = model.source
    sourceView.isEditable = !model.sourceIsPlaceholder
    // Undo steps recorded against the old text would act on the new one.
    sourceView.undoManager?.removeAllActions()
  }

  // The rows and the side switch follow the shown selection. The source pane is left alone.
  private func showSelection() {
    applying = true
    select(categoryName, in: categoryTable, values: model.categories)
    select(selectedClass, in: classTable, values: model.classes)
    if let protocolName {
      select(protocolName, in: protocolTable, values: model.protocols)
    } else {
      protocolTable.deselectAll(nil)
    }
    if let selectorName {
      select(selectorName, in: selectorTable, values: model.selectors)
    } else {
      selectorTable.deselectAll(nil)
    }
    sideControl.selectedSegment = meta ? 1 : 0
    applying = false
  }

  private func select(_ name: String, in table: NSTableView, values: [String]) {
    guard let row = values.firstIndex(of: name) else {
      table.deselectAll(nil)
      return
    }
    table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
  }

  private func rows(of table: NSTableView) -> [String] {
    if table === categoryTable {
      return model.categories
    }
    if table === classTable {
      return model.classes
    }
    if table === protocolTable {
      return model.protocols
    }
    if table === selectorTable {
      return model.selectors
    }
    return []
  }

  private func value(at row: Int, in values: [String]) -> String? {
    guard row >= 0, row < values.count else {
      return nil
    }
    return values[row]
  }

  private func configure(_ table: NSTableView) {
    let column = NSTableColumn(identifier: NSUserInterfaceItemIdentifier("name"))
    column.resizingMask = .autoresizingMask
    column.width = 240
    table.addTableColumn(column)
    table.headerView = nil
    table.allowsEmptySelection = true
    table.allowsMultipleSelection = false
    table.columnAutoresizingStyle = .lastColumnOnlyAutoresizingStyle
    table.dataSource = self
    table.delegate = self
  }

  private func scroll(for table: NSTableView, frame: NSRect) -> NSScrollView {
    let scroll = NSScrollView(frame: frame)
    scroll.hasVerticalScroller = true
    scroll.hasHorizontalScroller = false
    scroll.borderType = .noBorder
    scroll.autoresizingMask = [.width, .height]
    let contentSize = scroll.contentSize
    let width = contentSize.width > 0 ? contentSize.width : frame.width
    let height = contentSize.height > 0 ? contentSize.height : frame.height
    table.frame = NSRect(x: 0, y: 0, width: width, height: height)
    table.autoresizingMask = [.width]
    scroll.documentView = table
    return scroll
  }

  private func classPane(frame: NSRect) -> NSView {
    let pane = NSView(frame: frame)
    let sideHeight: CGFloat = 24
    let gap: CGFloat = 4
    sideControl.frame = NSRect(
      x: 8,
      y: max(frame.height - sideHeight - gap, 0),
      width: min(180, max(frame.width - 16, 1)),
      height: sideHeight
    )
    sideControl.autoresizingMask = [.minYMargin, .maxXMargin]
    let listHeight = max(frame.height - sideHeight - gap * 2, 1)
    let list = scroll(
      for: classTable,
      frame: NSRect(x: 0, y: 0, width: frame.width, height: listHeight)
    )
    list.autoresizingMask = [.width, .height]
    pane.addSubview(sideControl)
    pane.addSubview(list)
    return pane
  }

  private static func makeSource(frame: NSRect) -> (scroll: NSScrollView, text: NSTextView) {
    let scroll = NSScrollView(frame: frame)
    scroll.hasVerticalScroller = true
    scroll.hasHorizontalScroller = false
    scroll.borderType = .noBorder
    scroll.autoresizingMask = [.width, .height]
    let contentSize = scroll.contentSize
    let width = contentSize.width > 0 ? contentSize.width : frame.width
    let height = contentSize.height > 0 ? contentSize.height : frame.height
    let text = NSTextView(frame: NSRect(x: 0, y: 0, width: width, height: height))
    text.minSize = NSSize(width: 0, height: height)
    text.maxSize = NSSize(
      width: CGFloat.greatestFiniteMagnitude,
      height: CGFloat.greatestFiniteMagnitude
    )
    text.isVerticallyResizable = true
    text.isHorizontallyResizable = false
    text.autoresizingMask = [.width]
    text.isEditable = true
    text.isSelectable = true
    text.isRichText = false
    configureSourceEditing(text)
    if let container = text.textContainer {
      container.containerSize = NSSize(width: width, height: CGFloat.greatestFiniteMagnitude)
      container.widthTracksTextView = true
    }
    scroll.documentView = text
    return (scroll, text)
  }

  // The macOS form of a destructive question: Discard first and destructive, which leaves the
  // sheet without a default button, so Return answers nothing; Cancel keeps the Escape key that
  // NSAlert gives a button titled Cancel.
  private static func askToDiscard(_ window: NSWindow, _ decide: @escaping @MainActor (Bool) -> Void) {
    let alert = NSAlert()
    alert.messageText = "Discard the changes you have not accepted?"
    alert.informativeText = "The source pane has edits that were not accepted."
    alert.addButton(withTitle: "Discard").hasDestructiveAction = true
    alert.addButton(withTitle: "Cancel")
    alert.beginSheetModal(for: window) { response in
      MainActor.assumeIsolated {
        decide(response == .alertFirstButtonReturn)
      }
    }
  }

  private static func makeErrorField() -> NSTextField {
    let field = NSTextField(labelWithString: "")
    field.font = NSFont.systemFont(ofSize: NSFont.smallSystemFontSize)
    field.textColor = .secondaryLabelColor
    field.lineBreakMode = .byTruncatingTail
    field.setAccessibilityLabel("Error")
    return field
  }

  // A selected protocol or selector sends a method; only the class with neither sends a class
  // definition.
  private var acceptsMethod: Bool {
    protocolName != nil || selectorName != nil
  }

  // Failure leaves sourceView.string alone. refresh runs only after AO_OK.
  private func submit(_ source: String, method: Bool) -> (status: Int32, message: String) {
    var err = AoSpan()
    let status: Int32
    if !method {
      status = source.withCString { src in
        withUnsafeMutablePointer(to: &err) { errPtr in
          ao_accept_class(src, errPtr)
        }
      }
    } else {
      let metaFlag: Int32 = meta ? 1 : 0
      let className = selectedClass
      status = className.withCString { name in
        source.withCString { src in
          withUnsafeMutablePointer(to: &err) { errPtr in
            ao_accept_method(name, metaFlag, src, errPtr)
          }
        }
      }
    }
    return (status, spanMessage(err))
  }
}

// The tokens BrowserWindow.definedClassName needs, read as compiler/src/Scanner.cpp reads them: a
// comment is skipped, and a string, symbol or character literal is one token, so a subclass:
// inside one is not a keyword.
private struct DefinitionScanner {
  enum Token: Equatable {
    case keyword(String)
    // An identifier, a symbol or a string: the tokens whose text the chunk parser takes as a
    // definition keyword's argument.
    case value(String)
    case other
  }

  private let chars: [Unicode.Scalar]
  private var index = 0

  init(_ source: String) {
    chars = Array(source.unicodeScalars)
  }

  // nil at the end, and where the Scanner answers an error token and stops: a comment or string
  // that does not end, or a lone # or $.
  mutating func next() -> Token? {
    guard skipTrivia(), let c = peek() else {
      return nil
    }
    if isLetter(c) {
      let word = readWord()
      if peek() == ":" && peek(1) != "=" {
        index += 1
        return .keyword(word + ":")
      }
      return .value(word)
    }
    switch c {
    case "'":
      return readString().map(Token.value)
    case "#":
      index += 1
      return readSymbol()
    case "$":
      // One character, whatever it is ($' and $" too).
      guard index + 1 < chars.count else {
        return nil
      }
      index += 2
      return .other
    default:
      index += 1
      return .other
    }
  }

  private func peek(_ offset: Int = 0) -> Unicode.Scalar? {
    index + offset < chars.count ? chars[index + offset] : nil
  }

  private func isLetter(_ c: Unicode.Scalar) -> Bool {
    ("A"..."Z").contains(c) || ("a"..."z").contains(c)
  }

  private func isLetterOrDigit(_ c: Unicode.Scalar) -> Bool {
    isLetter(c) || ("0"..."9").contains(c)
  }

  // False inside a comment that does not end.
  private mutating func skipTrivia() -> Bool {
    while let c = peek() {
      if [" ", "\t", "\n", "\r", "\u{0C}", "\u{0B}"].contains(c) {
        index += 1
        continue
      }
      guard c == "\"" else {
        return true
      }
      guard let end = chars[(index + 1)...].firstIndex(of: "\"") else {
        return false
      }
      index = end + 1
    }
    return true
  }

  // At a letter.
  private mutating func readWord() -> String {
    var word = ""
    while let c = peek(), isLetterOrDigit(c) {
      word.unicodeScalars.append(c)
      index += 1
    }
    return word
  }

  // At the opening quote. '' inside is one quote.
  private mutating func readString() -> String? {
    var text = ""
    index += 1
    while let c = peek() {
      index += 1
      guard c == "'" else {
        text.unicodeScalars.append(c)
        continue
      }
      guard peek() == "'" else {
        return text
      }
      text.unicodeScalars.append(c)
      index += 1
    }
    return nil
  }

  // After the #: #'text', #name or #key:words: is a value. #( #[ and a binary selector are not,
  // and the token after the # is read on its own.
  private mutating func readSymbol() -> Token? {
    guard let c = peek() else {
      return nil
    }
    if c == "'" {
      return readString().map(Token.value)
    }
    guard isLetter(c) else {
      return .other
    }
    var text = ""
    while let part = peek(), isLetter(part) {
      text += readWord()
      guard peek() == ":" else {
        break
      }
      text += ":"
      index += 1
    }
    return .value(text)
  }
}

@MainActor
func sendToKeyBrowser(
  _ browser: BrowserWindow?,
  keyWindow: NSWindow?,
  _ command: (BrowserWindow) -> Void
) {
  guard let browser, browser.ownsWindow(keyWindow) else {
    return
  }
  command(browser)
}
