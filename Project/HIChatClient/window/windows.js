const electron = require('electron')
const BrowserWindow = electron.BrowserWindow
const path = require('path')
const app = electron.app

exports.mainWindow = null;

exports.createMainWindow = function() {
  var win = new BrowserWindow({ width: 850, height: 600, minWidth:850, minHeight:600, frame: false })
  win.loadURL(path.join('file://', __dirname, '../assets/main.html'))
  win.on('close', function () { win = null; })

  if (this.mainWindow) this.mainWindow.close();
  this.mainWindow = win;
}

exports.createAuthWindow = function() {
  var win = new BrowserWindow({ width: 350, height: 400, resizable: false, frame: false })
  win.loadURL(path.join('file://', __dirname, '../assets/auth.html'))
  win.on('close', function () { win = null })

  if (this.mainWindow) this.mainWindow.close();
  this.mainWindow = win;
  //win.toggleDevTools();
}