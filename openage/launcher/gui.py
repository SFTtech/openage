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
    Image widget with antialiased scaling
    """
    def __init__(self, image_path, max_x, max_y, *args, **kwargs):
        super().__init__(*args, **kwargs)
        pixmap = QPixmap(image_path).scaled(max_x, max_y, aspectRatioMode=Qt.KeepAspectRatio,
                                            transformMode=Qt.SmoothTransformation)
        self.setPixmap(pixmap)


class NiceButton(QPushButton):
    """
    Good-looking button
    """
    def __init__(self, text, *args, icon=None, background=None, text_size=10, **kwargs):
        # TODO: implement background
        super().__init__(*args, **kwargs)

        # configure font
        fixedfont = QFontDatabase.systemFont(QFontDatabase.FixedFont)
        fixedfont.setPointSize(text_size)
        self.setFont(fixedfont)

        if icon is not None:
            self.setIcon(QIcon(icon))
            self.setIconSize(QSize(30, 30))
        
        self.setText(text)


class MenuButton(NiceButton):
    """
    Menu button for the openage launcher
    """
    def __init__(self, *args, **kwargs):
        super().__init__(*args, text_size=15, **kwargs)
        self.setFixedSize(QSize(250, 50))


class ContentTable(QTableWidget):
    """
    Displays a table of arbitrary contents and allows
    check selection of its members.
    """
    # create and add table
    def __init__(self, *args, noselect=False, **kwargs):
        super().__init__(*args, **kwargs)

        # table settings, hardcoded for now TODO
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

    def load_content(self, dir_path):
        """
        Load all the available content into the table
        """
        # TODO
        # for thing in dir:
        #     unpack stuff
        #     self.add_entry(*stuff)


class NewsTab(QTextBrowser):
    """
    rich text browser for openage news with hyperlinks
    """
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setOpenExternalLinks(True)
        self.content = self.fetch_news()
        self.setText(self.content)

    def fetch_news(self):
        # TODO get news feed from somewhere
        feed = ('<p>Lorem ipsum dolor sit amet, consectetuer adipiscing '
                'elit. Aenean commodo ligula eget dolor. Aenean massa '
                '<strong>strong</strong>. Cum sociis natoque penatibus '
                'et magnis dis parturient montes, nascetur ridiculus '
                'mus.</p>') * 10

        # show it
        return ('<p><a href="https://github.com/SFTtech/openage/">Check out the Openage Github</a></p>'
                '<p>Any HTML should work here.</p>' + feed)


class ContentTab(QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setLayout(QVBoxLayout())

        self.table = ContentTable(noselect=True)
        self.layout().addWidget(self.table)

        # test content
        self.table.add_entry('testcontent0', '2.0', 'Someone', False)
        self.table.add_entry('testcontent1', '1.0', 'Someone Else', True)


class ModTab(QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setLayout(QVBoxLayout())

        # create and add table
        self.table = ContentTable()
        self.layout().addWidget(self.table)

        # create and add button layout
        self.buttons = QWidget()
        self.layout().addWidget(self.buttons)
        self.buttons.setLayout(QHBoxLayout())

        # create and add buttons
        self.buttons.up = NiceButton('Up')
        self.buttons.down = NiceButton('Down')
        self.buttons.add = NiceButton('Add')
        self.buttons.delete = NiceButton('Delete')

        self.buttons.layout().addWidget(self.buttons.up)
        self.buttons.layout().addWidget(self.buttons.down)
        self.buttons.layout().addStretch(1)
        self.buttons.layout().addWidget(self.buttons.add)
        self.buttons.layout().addWidget(self.buttons.delete)

        # connect buttons to methods
        self.buttons.up.clicked.connect(lambda: self.move_mod(-1))
        self.buttons.down.clicked.connect(lambda: self.move_mod(1))
        self.buttons.add.clicked.connect(self.add_mod)
        self.buttons.delete.clicked.connect(self.delete_mod)

        # test mods
        self.table.add_entry('testmod0', '2.0', 'Someone', False)
        self.table.add_entry('testmod1', '1.0', 'Someone Else', True)

    def selected_row(self):
        if not self.table.selectedItems():
            return None
        return self.table.selectedItems()[1].text(), self.table.selectedItems()[1].row()

    def move_mod(self, step):
        sel = self.selected_row()
        if sel is None:
            return
        print('mod {}, row {}, changed priority by {}'.format(*sel, step))

    def add_mod(self):
        # TODO should adding from local and browsing repo happen from the same spot?
        print('add a mod')

    def delete_mod(self):
        sel = self.selected_row()
        if sel is None:
            return
        print('delete selected mod: {}, row {}'.format(*sel))


class MainMenu(QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setLayout(QVBoxLayout())

        buttons = [
            ('Continue', self.continue_),
            ('Play', self.play_),
            ('Editor', self.editor_),
            ('Options', self.options_),
            ('Search for Updates', self.search_for_updates_),
            ('Quit', self.quit_)
        ]

        background = None
        for button, action in buttons:
            button_widget = MenuButton(button, icon=icon_path, background=background)
            button_widget.clicked.connect(action)
            self.layout().addWidget(button_widget)
            self.layout().addStretch(1)

    def fake_action(self, text):
        dialog = QErrorMessage(self)
        dialog.showMessage(text)

    def continue_(self):
        self.fake_action('Continue!')

    def play_(self):
        self.fake_action('Play!')

    def editor_(self):
        self.fake_action('Editor!')

    def options_(self):
        self.fake_action('Options!')

    def search_for_updates_(self):
        self.fake_action('Update!')

    def quit_(self):
        # TODO not sure this is a good way to do it
        QApplication.quit()


class LauncherWindow(QMainWindow):
    title = 'Openage Launcher'

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        # initialize version and run checks and test
        self.version = self.get_version()
        if self.is_outdated():
            self.update()

        
        

        # construct the ui layouts and populate them
        self.build_ui()


    def build_ui(self):
        self.setWindowTitle(self.title)
        self.setWindowIcon(QIcon(icon_path))
        self.setMinimumSize(QSize(700, 450))

        # main widget and main layout
        self.main_widget = QWidget()
        self.main_widget.setLayout(QHBoxLayout())
        self.setCentralWidget(self.main_widget)

        # various sub-layouts and their relative positioning
        self.left_widget = QWidget()
        self.left_widget.setLayout(QVBoxLayout())
        self.main_widget.layout().addWidget(self.left_widget)

        self.right_widget = QWidget()
        self.right_widget.setLayout(QVBoxLayout())
        self.main_widget.layout().addWidget(self.right_widget)

        # left side
        self.logo_widget = ImageLabel(logo_path, 250, 250)
        self.left_widget.layout().addWidget(self.logo_widget)

        self.left_widget.layout().addStretch(5)

        self.menu_widget = MainMenu()
        self.left_widget.layout().addWidget(self.menu_widget)
        self.left_widget.layout().addStretch(10)

        self.version_widget = QLabel(self.version)
        self.left_widget.layout().addWidget(self.version_widget)

        # right side
        self.tabs_widget = QTabWidget()
        self.right_widget.layout().addWidget(self.tabs_widget)

        self.news_widget = NewsTab()
        self.tabs_widget.addTab(self.news_widget, 'News')

        self.content_widget = ContentTab()
        self.tabs_widget.addTab(self.content_widget, 'Content')

        self.mods_widget = ModTab()
        self.tabs_widget.addTab(self.mods_widget, 'Mods')

        self.show()

    def get_version(self):
        return 'dummy version number'

    def is_outdated(self):
        latest = 'dummy version number'
        if self.version != latest:
            return True
        return False

    def run_tests(self):


    def update(self):
        print('updating to latest version')


if __name__ == '__main__':
    app = QApplication(sys.argv)
    app.setApplicationName('Openage Launcher')
    launcher_window = LauncherWindow()

    # catch KeyboardInterrupt
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    app.exec_()
