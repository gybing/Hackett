const electron = require('electron')
const BrowserWindow = electron.BrowserWindow
const Menu = electron.Menu
const MenuItem = electron.MenuItem
const ipc = electron.ipcMain
const app = electron.app

const menu = new Menu()
menu.append(new MenuItem({ label: 'Reload', click: function (menuItem,browserWindow) {if (browserWindow) { browserWindow.reload() }} }))
menu.append(new MenuItem({ label: 'DevTools', click: function (menuItem,browserWindow) {if (browserWindow) { browserWindow.toggleDevTools() }} }))

app.on('browser-window-created', function (event, win) {
  win.webContents.on('context-menu', function (e, params) {
    menu.popup(win, params.x, params.y)
  })
})

ipc.on('show-context-menu', function (event) {
  const win = BrowserWindow.fromWebContents(event.sender)
  menu.popup(win)
})
