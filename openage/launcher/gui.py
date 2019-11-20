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


class ModTable(QTableWidget):
    def __init__(self, *args, **kwargs):
        super(ModTable, self).__init__(*args, **kwargs)
        self.setColumnCount(4)
        self.setHorizontalHeaderLabels(['Activate', 'Name', 'Version', 'Author'])
        self.horizontalHeader().setStretchLastSection(True)
        self.verticalHeader().hide()

        # disable editability and selectability
        self.setSelectionMode(QTableWidget.NoSelection)
        self.setFocusPolicy(Qt.NoFocus)
        self.setEditTriggers(QAbstractItemView.NoEditTriggers)

        self.mods = []

        self.add_mod('testmod0', '2.0', 'Someone', False)
        self.add_mod('testmod1', '1.0', 'Someone Else', True)

    def add_mod(self, name, version, author, active):
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


class ModEditButtons(QWidget):
    def __init__(self, *args, **kwargs):
        super(ModEditButtons, self).__init__(*args, **kwargs)
        self.setLayout(QHBoxLayout())

        self.up = QPushButton('Up')
        self.down = QPushButton('Down')
        self.add = QPushButton('Add')
        self.delete = QPushButton('Delete')

        self.layout().addWidget(self.up)
        self.layout().addWidget(self.down)
        self.layout().addStretch(1)
        self.layout().addWidget(self.add)
        self.layout().addWidget(self.delete)


class LauncherWindow(QMainWindow):
    title = 'Openage Launcher'

    def __init__(self, *args, **kwargs):
        super(LauncherWindow, self).__init__(*args, **kwargs)
        self.setWindowTitle(LauncherWindow.title)
        self.setWindowIcon(QIcon(icon_path))
        self.setMinimumSize(QSize(700, 450))

        # main widget and main layout
        self.main = QWidget()
        self.main_layout = QHBoxLayout()
        self.main.setLayout(self.main_layout)
        self.setCentralWidget(self.main)

        # various sub-layouts and their relative positioning
        self.menu_layout = QVBoxLayout()
        self.main_layout.addLayout(self.menu_layout)

        self.tabs_layout = QVBoxLayout()
        self.main_layout.addLayout(self.tabs_layout)

        self._init_menu()
        self._init_tabs()

        self.show()

    def _init_menu(self):
        self.logo = ImageLabel(logo_path, 250, 250)
        self.menu_layout.addWidget(self.logo)

        self.menu_layout.addStretch(5)

        buttons = [
            ('Continue', self._continue),
            ('Play', self._play),
            ('Options', self._options),
            ('Search for Updates', self._search_for_updates),
            ('Quit', self._quit)
        ]

        background = None
        for button, action in buttons:
            button_widget = MenuButton(button, icon_path, background)
            button_widget.clicked.connect(action)
            self.menu_layout.addWidget(button_widget)
            self.menu_layout.addStretch(1)

        self.menu_layout.addStretch(10)

        version = 'dummy version number'

        self.version = QLabel(version)
        self.menu_layout.addWidget(self.version)

    def _init_tabs(self):
        self.tabs = QTabWidget()
        self.tabs_layout.addWidget(self.tabs)

        self.news = NewsBox()
        self.tabs.addTab(self.news, 'News')

        self.mods = QWidget()
        self.mods.setLayout(QVBoxLayout())
        self.mods.layout().addWidget(ModTable())
        self.mods.layout().addWidget(ModEditButtons())

        self.tabs.addTab(self.mods, 'Mods')

    def fake_action(self, text):
        dialog = QErrorMessage(self)
        dialog.showMessage(text)

    def _continue(self):
        self.fake_action('Continue!')

    def _play(self):
        self.fake_action('Play!')

    def _options(self):
        self.fake_action('Options!')

    def _search_for_updates(self):
        self.fake_action('Update!')

    def _quit(self):
        self.close()


if __name__ == '__main__':
    app = QApplication(sys.argv)

    launcher_window = LauncherWindow()

    # catch KeyboardInterrupt
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    app.exec_()
