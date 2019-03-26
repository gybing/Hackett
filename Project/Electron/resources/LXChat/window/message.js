'use strict';

const ipcMain = require('electron').ipcMain;
const window = require('./windows')
const dialog = require('electron').dialog

const init = () => {
  ipcMain.on('message-login', function(event, arg) {
    window.createMainWindow()
  });
  
  ipcMain.on('message-logout', function(event, arg) {
    window.createAuthWindow()
  });
  
  ipcMain.on('message-close', function(event, arg) {
    if (window.mainWindow)
      window.mainWindow.close();
    window.mainWindow = null;
  });
  ipcMain.on('message-minsize', function(event, arg) {
    if (window.mainWindow)
      window.mainWindow.minimize();
  });
}

module.exports = { init: init };