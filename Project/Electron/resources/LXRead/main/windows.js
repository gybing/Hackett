const electron = require('electron')
const BrowserWindow = electron.BrowserWindow
const path = require('path')
const app = electron.app

exports.mainWindow = null;
exports.bookWindow = null;
exports.createMainWindow = function() {
  var win = new BrowserWindow({ width: 1280, height: 800, frame: false })
  win.loadURL(path.join('file://', __dirname, '/main/index.html'))
  win.on('close', function () {
    win = null;
    if (this.bookWindow) this.bookWindow.close();
      this.bookWindow = win;
  })

  if (this.mainWindow) this.mainWindow.close();
  this.mainWindow = win;
}

exports.createAuthWindow = function() {
  var win = new BrowserWindow({ width: 600, height: 400, resizable: false, frame: false })
  win.loadURL(path.join('file://', __dirname, '/auth/index.html'))
  win.on('close', function () { win = null })

  if (this.mainWindow) this.mainWindow.close();
  this.mainWindow = win;
  this.createBookWindow();
}

exports.createBookWindow = function() {
  if (this.bookWindow == null){
    this.bookWindow = new BrowserWindow();
    this.bookWindow.loadURL(path.join('file://', __dirname, '../book/index.html'))
    this.bookWindow.on('close', function () { this.bookWindow = null })
    this.bookWindow.show();
  }
}