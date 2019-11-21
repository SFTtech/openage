# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Game Launcher
"""

import sys
import signal
from PyQt5.QtGui import QFontDatabase, QIcon, QPixmap
from PyQt5.QtWidgets import QApplication, QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, \
    QPushButton, QLabel, QTabWidget, QErrorMessage, QTextBrowser, QTableWidget, \
    QTableWidgetItem, QAbstractItemView
from PyQt5.QtCore import QSize, Qt


# assets
logo_path = '../../assets/logo/banner.png'
icon_path = '../../assets/logo/favicon.ico'


class ImageLabel(QLabel):
    """
    image widget with antialiased scaling
    """
    def __init__(self, image_path, max_x, max_y, *args, **kwargs):
        super(ImageLabel, self).__init__(*args, **kwargs)
        pixmap = QPixmap(image_path).scaled(max_x, max_y, aspectRatioMode=Qt.KeepAspectRatio,
                                            transformMode=Qt.SmoothTransformation)
        self.setPixmap(pixmap)


class MenuButton(QPushButton):
    """
    main menu button class for the openage launcher
    """
    def __init__(self, text, icon, background, *args, **kwargs):
        super(MenuButton, self).__init__(*args, **kwargs)
        self.setFixedSize(QSize(250, 50))

        # configure font
        fixedfont = QFontDatabase.systemFont(QFontDatabase.FixedFont)
        fixedfont.setPointSize(15)
        self.setFont(fixedfont)

        self.setIcon(QIcon(icon))
        self.setIconSize(QSize(30, 30))
        self.setText(text)

        # background?


class NewsBox(QTextBrowser):
    """
    rich text browser for openage news with hyperlink
    """
    def __init__(self, *args, **kwargs):
        super(NewsBox, self).__init__(*args, **kwargs)
        self.setOpenExternalLinks(True)

        self.setText('<p><a href="https://github.com/SFTtech/openage/">Check out the Openage Github</a></p>'
                     '<p>Any HTML should work here.</p>')

        dummy = ('<p>Lorem ipsum dolor sit amet, consectetuer adipiscing '
                 'elit. Aenean commodo ligula eget dolor. Aenean massa '
                 '<strong>strong</strong>. Cum sociis natoque penatibus '
                 'et magnis dis parturient montes, nascetur ridiculus '
                 'mus.</p>')

        for _ in range(10):
            self.append(dummy)


class ContentTable(QTableWidget):
    # create and add table
    def __init__(self, *args, noselect=False, **kwargs):
        super(ContentTable, self).__init__(*args, **kwargs)

        # table settings
        self.setColumnCount(4)
        self.setHorizontalHeaderLabels(['Activate', 'Name', 'Version', 'Author'])
        self.horizontalHeader().setStretchLastSection(True)
        self.verticalHeader().hide()
        # disable editability, enable row selection
        if noselect:
            self.setSelectionMode(QAbstractItemView.NoSelection)
        else:
            self.setSelectionMode(QTableWidget.SingleSelection)
            self.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.setFocusPolicy(Qt.NoFocus)
        self.setEditTriggers(QAbstractItemView.NoEditTriggers)

    def add_entry(self, name, version, author, active):
        """
        add an entry to the table and display its activation state
        """
        idx = self.rowCount()
        self.insertRow(idx)
        check = QTableWidgetItem()
        check.setCheckState(Qt.Unchecked)
        check.setTextAlignment(Qt.AlignCenter)
        if active:
            check.setCheckState(Qt.Checked)
        self.setItem(idx, 0, check)
        self.setItem(idx, 1, QTableWidgetItem(name))
        self.setItem(idx, 2, QTableWidgetItem(version))
        self.setItem(idx, 3, QTableWidgetItem(author))


class ContentBox(QWidget):
    def __init__(self, *args, **kwargs):
        super(ContentBox, self).__init__(*args, **kwargs)
        self.setLayout(QVBoxLayout())

        self.table = ContentTable(noselect=True)
        self.layout().addWidget(self.table)

        # test content
        self.table.add_entry('testcontent0', '2.0', 'Someone', False)
        self.table.add_entry('testcontent1', '1.0', 'Someone Else', True)

class ModBox(QWidget):
    def __init__(self, *args, **kwargs):
        super(ModBox, self).__init__(*args, **kwargs)
        self.setLayout(QVBoxLayout())

        # create and add table
        self.table = ContentTable()
        self.layout().addWidget(self.table)

        # create and add button layout
        self.buttons = QWidget()
        self.layout().addWidget(self.buttons)
        self.buttons.setLayout(QHBoxLayout())

        # create and add buttons
        self.buttons.up = QPushButton('Up')
        self.buttons.down = QPushButton('Down')
        self.buttons.add = QPushButton('Add')
        self.buttons.delete = QPushButton('Delete')

        self.buttons.layout().addWidget(self.buttons.up)
        self.buttons.layout().addWidget(self.buttons.down)
        self.buttons.layout().addStretch(1)
        self.buttons.layout().addWidget(self.buttons.add)
        self.buttons.layout().addWidget(self.buttons.delete)

        # connect buttons to methods
        self.buttons.up.pressed.connect(self.mod_up)
        self.buttons.down.pressed.connect(self.mod_down)

        # test mods
        self.table.add_entry('testmod0', '2.0', 'Someone', False)
        self.table.add_entry('testmod1', '1.0', 'Someone Else', True)

    def _move_mod(self, direction):
        pass

    def mod_up(self):
        """
        increase mod priority
        """
        self._move_mod('up')

    def mod_down(self):
        """
        decrease mod priority
        """
        self._move_mod('down')


class LauncherWindow(QMainWindow):
    title = 'Openage Launcher'

    def __init__(self, *args, **kwargs):
        super(LauncherWindow, self).__init__(*args, **kwargs)
        self.setWindowTitle(LauncherWindow.title)
        self.setWindowIcon(QIcon(icon_path))
        self.setMinimumSize(QSize(700, 450))

        # main widget and main layout
        self.main = QWidget()
        self.main.setLayout(QHBoxLayout())
        self.setCentralWidget(self.main)

        # various sub-layouts and their relative positioning
        self.left = QWidget()
        self.left.setLayout(QVBoxLayout())
        self.main.layout().addWidget(self.left)

        self.right = QWidget()
        self.right.setLayout(QVBoxLayout())
        self.main.layout().addWidget(self.right)

        self._init_menu()
        self._init_tabs()

        self.show()

    def _init_menu(self):
        self.logo = ImageLabel(logo_path, 250, 250)
        self.left.layout().addWidget(self.logo)

        self.left.layout().addStretch(5)

        buttons = [
            ('Continue', self.continue_),
            ('Play', self.play),
            ('Options', self.options),
            ('Search for Updates', self.search_for_updates),
            ('Quit', self.quit)
        ]

        background = None
        for button, action in buttons:
            button_widget = MenuButton(button, icon_path, background)
            button_widget.clicked.connect(action)
            self.left.layout().addWidget(button_widget)
            self.left.layout().addStretch(1)

        self.left.layout().addStretch(10)

        version = 'dummy version number'

        self.version = QLabel(version)
        self.left.layout().addWidget(self.version)

    def _init_tabs(self):
        self.tabs = QTabWidget()
        self.right.layout().addWidget(self.tabs)

        self.news = NewsBox()
        self.tabs.addTab(self.news, 'News')

        self.content = ContentBox()
        self.tabs.addTab(self.content, 'Content')

        self.mods = ModBox()
        self.tabs.addTab(self.mods, 'Mods')

    def fake_action(self, text):
        dialog = QErrorMessage(self)
        dialog.showMessage(text)

    def continue_(self):
        self.fake_action('Continue!')

    def play(self):
        self.fake_action('Play!')

    def options(self):
        self.fake_action('Options!')

    def search_for_updates(self):
        self.fake_action('Update!')

    def quit(self):
        self.close()


if __name__ == '__main__':
    app = QApplication(sys.argv)

    launcher_window = LauncherWindow()

    # catch KeyboardInterrupt
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    app.exec_()
