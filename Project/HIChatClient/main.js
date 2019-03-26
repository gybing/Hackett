// main.js

(function () {
    const {app} = require('electron');
    var windows = require('./window/windows');

    // 只允许启动一个应用程序实例

    // 模块导入

    // 自动更新
    var checkUpdate = require('./comm/check_update');

    // 程序入口
    var main = () => {
        'use strict';

        checkUpdate();

        windows.createMainWindow();

        require('./menu/context-menu');
        require('./window/message').init();
    };

    app.on('ready', main);

    module.exports = main;

})();