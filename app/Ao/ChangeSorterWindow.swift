import AppKit
import CAo

// SPEC §3.9 Change Sorter (P16): the change sets, the selected set's classes, the selected class's
// methods, and a read-only source pane. isVertical is a vertical divider, so side-by-side lists.
@MainActor
final class ChangeSorterWindow: NSObject, NSTableViewDataSource, NSTableViewDelegate, NSMenuDelegate,
  NSWindowDelegate {
  let model = ChangeSorterModel()
  let window: NSWindow

  private let setTable = NSTableView()
  private let classTable = NSTableView()
  private let methodTable = NSTableView()
  private let sourceView: NSTextView
  private let uniformFont = UniformFont()
  private let errorField: NSTextField
  private let newButton: NSButton
  private let currentButton: NSButton
  private let fileOutButton: NSButton
  private var applying = false

  // Asked for the new set's name, with the suggested one; the callback gets nil for Cancel. Tests
  // replace it.
  typealias NameQuestion = @MainActor (NSWindow, String, @escaping @MainActor (String?) -> Void) -> Void
  var askName: NameQuestion = ChangeSorterWindow.askForName

  // Asked where File Out… writes, with the suggested file name; the callback gets nil for Cancel.
  // Tests replace it.
  typealias FileOutQuestion = @MainActor (NSWindow, String, @escaping @MainActor (URL?) -> Void) -> Void
  var chooseFileOut: FileOutQuestion = ChangeSorterWindow.askForFileOutURL

  var sourceText: String {
    sourceView.string
  }

  var errorText: String {
    errorField.stringValue
  }

  var setTitles: [String] {
    rowTitles(for: setTable)
  }

  var classTitles: [String] {
    rowTitles(for: classTable)
  }

  var methodTitles: [String] {
    rowTitles(for: methodTable)
  }

  var canMakeCurrent: Bool {
    currentButton.isEnabled
  }

  var paneAccessibilityLabels: [String?] {
    [
      setTable.accessibilityLabel(),
      classTable.accessibilityLabel(),
      methodTable.accessibilityLabel(),
      sourceView.accessibilityLabel(),
      errorField.accessibilityLabel(),
    ]
  }

  override init() {
    _ = NSApplication.shared
    let frame = NSRect(x: 260, y: 140, width: 760, height: 520)
    let footer: CGFloat = 36
    // NSSplitView keeps the current size ratio, so a zero frame stays collapsed.
    let band = NSRect(x: 0, y: 0, width: frame.width, height: (frame.height - footer) / 2)
    let column = NSRect(x: 0, y: 0, width: frame.width / 3, height: band.height)
    let window = NSWindow(
      contentRect: frame,
      styleMask: [.titled, .closable, .miniaturizable, .resizable],
      backing: .buffered,
      defer: false
    )
    window.title = "Change Sorter"
    window.isReleasedWhenClosed = false
    window.isRestorable = false
    let source = ChangeSorterWindow.makeSource(frame: band)
    let errorField = NSTextField(labelWithString: "")
    errorField.font = NSFont.systemFont(ofSize: NSFont.smallSystemFontSize)
    errorField.textColor = .secondaryLabelColor
    errorField.lineBreakMode = .byTruncatingTail

    self.window = window
    self.sourceView = source.text
    self.errorField = errorField
    self.newButton = NSButton(title: "New…", target: nil, action: nil)
    self.currentButton = NSButton(title: "Make Current", target: nil, action: nil)
    self.fileOutButton = NSButton(title: "File Out…", target: nil, action: nil)
    super.init()

    configure(setTable)
    configure(classTable)
    configure(methodTable)
    classTable.menu = entryContextMenu()
    methodTable.menu = entryContextMenu()
    setTable.setAccessibilityLabel("Change sets")
    classTable.setAccessibilityLabel("Classes")
    methodTable.setAccessibilityLabel("Methods")
    sourceView.setAccessibilityLabel("Source")
    errorField.setAccessibilityLabel("Error")
    newButton.target = self
    newButton.action = #selector(newSetPressed(_:))
    currentButton.target = self
    currentButton.action = #selector(makeCurrentPressed(_:))
    fileOutButton.target = self
    fileOutButton.action = #selector(fileOutPressed(_:))

    let lists = NSSplitView(frame: band)
    lists.isVertical = true
    lists.dividerStyle = .thin
    for table in [setTable, classTable, methodTable] {
      let pane = scroll(for: table, frame: column)
      lists.addArrangedSubview(pane)
      pane.frame = column
    }
    let outer = NSSplitView(
      frame: NSRect(x: 0, y: footer, width: frame.width, height: frame.height - footer))
    outer.isVertical = false
    outer.dividerStyle = .thin
    outer.autoresizingMask = [.width, .height]
    outer.addArrangedSubview(lists)
    outer.addArrangedSubview(source.scroll)
    lists.frame = band
    source.scroll.frame = band

    let content = NSView(frame: NSRect(origin: .zero, size: frame.size))
    content.addSubview(outer)
    var right = frame.width - 8
    for button in [fileOutButton, currentButton, newButton] {
      button.sizeToFit()
      right -= button.frame.width
      button.frame.origin = NSPoint(x: right, y: 6)
      button.autoresizingMask = [.minXMargin, .maxYMargin]
      content.addSubview(button)
      right -= 8
    }
    errorField.frame = NSRect(x: 8, y: 10, width: max(right - 16, 1), height: 16)
    errorField.autoresizingMask = [.width, .maxYMargin]
    content.addSubview(errorField)
    window.contentView = content
    window.delegate = self

    applyFont()
    reload()
    window.makeKeyAndOrderFront(nil)
  }

  // SPEC §3.9 Change Sorter: reads the runtime again and shows it.
  func reload() {
    model.refresh()
    show()
  }

  // SPEC §3.9 Change Sorter: a load started the sets over; the current set is selected.
  func noteImageLoaded() {
    model.select(setID: nil)
    errorField.stringValue = ""
    show()
  }

  func applyFont() {
    guard let font = ToolTextSize.font(fixedPitch: false) else {
      return
    }
    uniformFont.apply(font, to: sourceView)
  }

  func windowDidBecomeKey(_ notification: Notification) {
    reload()
  }

  // Selects rows as clicks would.
  func selectRows(set: Int, class classRow: Int? = nil, method: Int? = nil) {
    let setID = value(at: set, in: model.sets)?.id
    model.select(setID: setID)
    let className = classRow.flatMap { value(at: $0, in: model.classes)?.name }
    model.select(setID: setID, className: className)
    model.select(
      setID: setID,
      className: className,
      method: method.flatMap { value(at: $0, in: model.methods) }
    )
    show()
  }

  func newSet() {
    askName(window, model.nextFreeName()) { [weak self] name in
      guard let self, let name else {
        return
      }
      self.report(self.model.newSet(named: name))
      self.show()
    }
  }

  func makeCurrent() {
    guard let id = model.selectedSetID else {
      return
    }
    let status = model.makeCurrent(id)
    report((status, "Make Current failed"))
    show()
  }

  func fileOut() {
    guard let id = model.selectedSetID, let name = model.selectedSetName else {
      return
    }
    chooseFileOut(window, name + ".st") { [weak self] url in
      guard let self, let url else {
        return
      }
      self.report(fileOutChangeSet(id, to: url))
    }
  }

  func removeEntry(className: String, method: ChangeMethodRow?) {
    report(model.remove(className: className, method: method))
    show()
  }

  func moveEntry(className: String, method: ChangeMethodRow?, to setID: Int64) {
    report(model.move(className: className, method: method, to: setID))
    show()
  }

  // SPEC §3.9 Change Sorter: Remove from Change Set, a separator, and Move to ▸ the other sets.
  // Without an entry, the items are disabled; without another set, so is Move to.
  func entryMenu(className: String?, method: ChangeMethodRow?) -> NSMenu {
    let menu = NSMenu(title: "Entry")
    fill(menu, className: className, method: method)
    return menu
  }

  // MARK: NSTableViewDataSource, NSTableViewDelegate

  func numberOfRows(in tableView: NSTableView) -> Int {
    rowTitles(for: tableView).count
  }

  func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView? {
    guard let text = value(at: row, in: rowTitles(for: tableView)) else {
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
    let setID = value(at: setTable.selectedRow, in: model.sets)?.id
    if table === setTable {
      model.select(setID: setID)
    } else if table === classTable {
      model.select(setID: setID, className: value(at: classTable.selectedRow, in: model.classes)?.name)
    } else {
      model.select(
        setID: setID,
        className: model.selectedClass,
        method: value(at: methodTable.selectedRow, in: model.methods)
      )
    }
    show()
  }

  // MARK: NSMenuDelegate

  // SPEC §3.9 Change Sorter: the right-clicked row's entry; nothing outside a row.
  func menuNeedsUpdate(_ menu: NSMenu) {
    if menu === classTable.menu {
      fill(menu, className: value(at: classTable.clickedRow, in: model.classes)?.name, method: nil)
      return
    }
    let method = value(at: methodTable.clickedRow, in: model.methods)
    fill(menu, className: method == nil ? nil : model.selectedClass, method: method)
  }

  // MARK: private

  private func show() {
    applying = true
    defer { applying = false }
    setTable.reloadData()
    classTable.reloadData()
    methodTable.reloadData()
    selectRow(model.sets.firstIndex { $0.id == model.selectedSetID }, in: setTable)
    selectRow(model.classes.firstIndex { $0.name == model.selectedClass }, in: classTable)
    selectRow(model.methods.firstIndex { $0 == model.selectedMethod }, in: methodTable)
    sourceView.string = model.source
    window.subtitle = model.currentName.map { "\($0) is current" } ?? ""
    currentButton.isEnabled = model.selectedSetID != nil && model.selectedSetID != model.currentID
    fileOutButton.isEnabled = model.selectedSetID != nil
  }

  private func report(_ result: (status: Int32, message: String)) {
    errorField.stringValue = result.status == Int32(AO_OK) ? "" : result.message
  }

  private func rowTitles(for table: NSTableView) -> [String] {
    if table === setTable {
      return model.sets.map { ($0.id == model.currentID ? "• " : "") + $0.name }
    }
    if table === classTable {
      return model.classes.map { titled($0.name, $0.kind) }
    }
    return model.methods.map { titled($0.label, $0.kind) }
  }

  private func titled(_ name: String, _ kind: ChangeKind) -> String {
    kind.mark.isEmpty ? name : "\(name) \(kind.mark)"
  }

  private func selectRow(_ row: Int?, in table: NSTableView) {
    if let row {
      table.selectRowIndexes(IndexSet(integer: row), byExtendingSelection: false)
    } else {
      table.deselectAll(nil)
    }
  }

  private func entryContextMenu() -> NSMenu {
    let menu = NSMenu(title: "Entry")
    menu.delegate = self
    return menu
  }

  private func fill(_ menu: NSMenu, className: String?, method: ChangeMethodRow?) {
    menu.removeAllItems()
    menu.autoenablesItems = false
    let remove = NSMenuItem(
      title: "Remove from Change Set", action: #selector(removeFromMenu(_:)), keyEquivalent: "")
    remove.target = self
    remove.isEnabled = className != nil
    remove.representedObject = className.map { EntryTarget(className: $0, method: method, setID: 0) }
    menu.addItem(remove)
    menu.addItem(.separator())
    let targets = NSMenu(title: "Move to")
    targets.autoenablesItems = false
    for set in model.sets where set.id != model.selectedSetID {
      let item = NSMenuItem(title: set.name, action: #selector(moveFromMenu(_:)), keyEquivalent: "")
      item.target = self
      item.isEnabled = className != nil
      item.representedObject = className.map { EntryTarget(className: $0, method: method, setID: set.id) }
      targets.addItem(item)
    }
    let move = NSMenuItem(title: "Move to", action: nil, keyEquivalent: "")
    move.submenu = targets
    move.isEnabled = className != nil && !targets.items.isEmpty
    menu.addItem(move)
  }

  @objc private func removeFromMenu(_ sender: NSMenuItem) {
    guard let target = sender.representedObject as? EntryTarget else {
      return
    }
    removeEntry(className: target.entryClass, method: target.method)
  }

  @objc private func moveFromMenu(_ sender: NSMenuItem) {
    guard let target = sender.representedObject as? EntryTarget else {
      return
    }
    moveEntry(className: target.entryClass, method: target.method, to: target.setID)
  }

  @objc private func newSetPressed(_ sender: Any?) {
    newSet()
  }

  @objc private func makeCurrentPressed(_ sender: Any?) {
    makeCurrent()
  }

  @objc private func fileOutPressed(_ sender: Any?) {
    fileOut()
  }

  private func value<Element>(at row: Int, in values: [Element]) -> Element? {
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
    text.maxSize = NSSize(width: CGFloat.greatestFiniteMagnitude, height: CGFloat.greatestFiniteMagnitude)
    text.isVerticallyResizable = true
    text.isHorizontallyResizable = false
    text.autoresizingMask = [.width]
    text.isEditable = false
    text.isSelectable = true
    text.isRichText = false
    if let container = text.textContainer {
      container.containerSize = NSSize(width: width, height: CGFloat.greatestFiniteMagnitude)
      container.widthTracksTextView = true
    }
    scroll.documentView = text
    return (scroll, text)
  }

  // SPEC §3.9 Change Sorter: `New Change Set`, a name field, Create and Cancel.
  static func askForName(
    _ window: NSWindow,
    _ suggested: String,
    _ answer: @escaping @MainActor (String?) -> Void
  ) {
    let alert = NSAlert()
    alert.messageText = "New Change Set"
    let field = NSTextField(string: suggested)
    field.frame = NSRect(x: 0, y: 0, width: 240, height: 24)
    field.setAccessibilityLabel("Name")
    alert.accessoryView = field
    alert.addButton(withTitle: "Create")
    alert.addButton(withTitle: "Cancel")
    alert.window.initialFirstResponder = field
    alert.beginSheetModal(for: window) { response in
      MainActor.assumeIsolated {
        answer(response == .alertFirstButtonReturn ? field.stringValue : nil)
      }
    }
  }

  static func askForFileOutURL(
    _ window: NSWindow,
    _ name: String,
    _ answer: @escaping @MainActor (URL?) -> Void
  ) {
    let panel = NSSavePanel()
    panel.canCreateDirectories = true
    panel.nameFieldStringValue = name
    panel.beginSheetModal(for: window) { response in
      MainActor.assumeIsolated {
        answer(response == .OK ? panel.url : nil)
      }
    }
  }
}

// A context menu item's entry and, for Move to, its target set.
private final class EntryTarget: NSObject {
  // Not `className`: NSObject already has one.
  let entryClass: String
  let method: ChangeMethodRow?
  let setID: Int64

  init(className: String, method: ChangeMethodRow?, setID: Int64) {
    self.entryClass = className
    self.method = method
    self.setID = setID
  }
}
