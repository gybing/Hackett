const storage = require('electron-json-storage')
const ipcRenderer = require('electron').ipcRenderer;

var book_content = document.getElementById("button-header-menu-store")
book_content.turn = turn({
  width: 400,
  height: 300,
  autoCenter: true
});
