const electron = require('electron')
const BrowserWindow = electron.BrowserWindow
const path = require('path')
const app = electron.app
const mainwindow = require('./main/windows')

const debug = 1///--debug/.test(process.argv[2])

if (process.mas) app.setName('乐学阅读')

function initialize () {
  var shouldQuit = makeSingleInstance()
  if (shouldQuit) return app.quit()

  loadModules();

  app.on('ready', function () {
    mainwindow.createAuthWindow();
  })

  app.on('window-all-closed', function () {
    if (process.platform !== 'darwin') {
      app.quit()
    }
  })
}

// Make this app a single instance app.
function makeSingleInstance () {
  if (process.mas) return false

  return app.makeSingleInstance(function () {
    if (mainwindow.mainWindow) {
      if (mainwindow.mainWindow.isMinimized()) mainwindow.mainWindow.restore()
      mainwindow.mainWindow.focus()
    }
  })
}

// Require each JS file in the main-process dir
function loadModules () {
  require('./main/context-menu')
  require('./main/message')
}

// Handle Squirrel on Windows startup events
switch (process.argv[1]) {
  default:
    initialize()
}
