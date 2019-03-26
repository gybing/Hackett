const storage = require('electron-json-storage')
const ipcRenderer = require('electron').ipcRenderer;

const store_urls = new Array(
  "http://c8.hanwenbook.com/?f=recom",
  "http://c8.hanwenbook.com/?f=library",
  "http://c8.hanwenbook.com/?f=bookrank"
);

const view = document.getElementById("webview-native-content");
const menus = new Array(
  document.getElementById("button-store-free"), 
  document.getElementById("button-store-type"), 
  document.getElementById("button-store-rank")
);

view.addEventListener("dom-ready", function() {
  const webContents = view.getWebContents()
  var code = "function onItemClick(arr){var ipc=require('electron').ipcRenderer; ipc.sendToHost('message-onitemclick', arr);}";
  webContents.executeJavaScript(code, (result) => {
  })

  // view.openDevTools();
});

view.addEventListener('did-start-loading', () => {
  const webContents = view.getWebContents()
  var code = "function onItemClick(arr){var ipc=require('electron').ipcRenderer; ipc.sendToHost('message-onitemclick', arr);}";
  webContents.executeJavaScript(code, (result) => {
  })

  storage.get('storage-data-auth-cookies', function (err, cookies) {
    Array.prototype.forEach.call(cookies, function (coo) {
      webContents.session.cookies.set({
        url: "http://.hanwenbook.com",
        name : coo['key'],
        value : coo['value']},
        function (error){if (error) throw error;
      });
    })
  })
})

Array.prototype.forEach.call(menus, function (btn, index) {
  btn.addEventListener('click', function (event) {
    activeStoreView(index);
    storage.set('storage-store-active', index, function (err) {
      if (err) return console.error(err)
    })
  })
})

storage.get('storage-store-active', function (err, id) {
  var act = 0;

  if (err) {
    console.error(err);
  }
  else if (id && id.length) {
      act = id;
  }
  activeStoreView(act);
})

function activeStoreView(idx){
  if (!menus[idx].classList.contains('is-selected')){
    for (var i = 0; i < 3; i++) {
      if (i == idx) {
        menus[i].classList.add('is-selected')
        view.src = store_urls[i];
      } else {
        menus[i].classList.remove('is-selected')
      }
    }
  }
}

