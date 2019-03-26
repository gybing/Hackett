const ipcMain = require('electron').ipcMain;
const window = require('./windows')
const dialog = require('electron').dialog

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

ipcMain.on('message-open-book', function(event, arg) {
  window.createBookWindow();
});

ipcMain.on('message-onitemclick', function(event, arg) {
  const options = {
      type: 'info',
      title: 'Renderer Process Hanging',
      message: 'This process is hanging.',
      buttons: ['Reload', 'Close']
    }
    dialog.showMessageBox(options, function (index) {
      if (index === 0) win.reload()
      else win.close()
    })
});