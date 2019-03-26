const storage = require('electron-json-storage')
const ipcRenderer = require('electron').ipcRenderer;

document.getElementById("button-header-close").addEventListener('click', function (event) {
  ipcRenderer.send('message-close', "success");
})

const menus = new Array(
  document.getElementById("button-header-menu-store"),
  document.getElementById("button-header-menu-shelf"),
  document.getElementById("button-header-menu-class"),
  document.getElementById("button-header-menu-home")
);

const views = document.querySelectorAll('.content-item');

Array.prototype.forEach.call(menus, function (btn, index) {
  btn.addEventListener('click', function (event) {
    activeMainContent(index);
    storage.set('storage-main-content-active', index, function (err) {
      if (err) return console.error(err)
    })
  })
})

storage.get('storage-main-content-active', function (err, id) {
  var act = 0;
  if (err) {
    console.error(err);
  }
  else if (id && id.length) {
      act = id;
  }
  activeMainContent(act);
})

function activeMainContent(idx){
  if (!menus[idx].classList.contains('is-selected')){
    for (var i = 0; i < 3; i++) {
      if (i == idx) {
        menus[i].classList.add('is-selected')
        views[i].classList.remove('is-hidden')
      } else {
        menus[i].classList.remove('is-selected')
        views[i].classList.add('is-hidden')
      }
    }
  }
}

