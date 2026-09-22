import AppKit

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
  private let sideControl: NSSegmentedControl

  private var categoryName = "Kernel"
  private var selectedClass = "Object"
  private var meta = false
  private var protocolName = "native"
  private var selectorName: String? = "printString"
  private var applying = false

  var title: String {
    window.title
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
    let sideControl = NSSegmentedControl(
      labels: ["instance", "class"],
      trackingMode: .selectOne,
      target: nil,
      action: nil
    )
    sideControl.selectedSegment = 0

    self.window = window
    self.sourceView = source.text
    self.sideControl = sideControl
    super.init()

    sideControl.target = self
    sideControl.action = #selector(sideChanged(_:))
    configure(categoryTable)
    configure(classTable)
    configure(protocolTable)
    configure(selectorTable)

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
    outer.addArrangedSubview(top)
    outer.addArrangedSubview(middle)
    outer.addArrangedSubview(source.scroll)
    top.frame = band
    middle.frame = band
    source.scroll.frame = band
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
    if table === categoryTable {
      if let name = value(at: table.selectedRow, in: model.categories) {
        categoryName = name
      }
      selectorName = nil
    } else if table === classTable {
      if let name = value(at: table.selectedRow, in: model.classes) {
        selectedClass = name
      }
      selectorName = nil
    } else if table === protocolTable {
      if let name = value(at: table.selectedRow, in: model.protocols) {
        protocolName = name
      }
      selectorName = nil
    } else if table === selectorTable {
      selectorName = value(at: table.selectedRow, in: model.selectors)
    } else {
      return
    }
    publish()
  }

  @objc private func sideChanged(_ sender: NSSegmentedControl) {
    guard !applying else {
      return
    }
    meta = sender.selectedSegment == 1
    selectorName = nil
    publish()
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
    model.select(
      category: categoryName,
      className: selectedClass,
      meta: meta,
      protocol: protocolName,
      selector: selectorName
    )
    reloadLists()
  }

  private func reloadLists() {
    applying = true
    categoryTable.reloadData()
    classTable.reloadData()
    protocolTable.reloadData()
    selectorTable.reloadData()
    select(categoryName, in: categoryTable, values: model.categories)
    select(selectedClass, in: classTable, values: model.classes)
    select(protocolName, in: protocolTable, values: model.protocols)
    if let selectorName {
      select(selectorName, in: selectorTable, values: model.selectors)
    } else {
      selectorTable.deselectAll(nil)
    }
    sourceView.string = model.source
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
}
