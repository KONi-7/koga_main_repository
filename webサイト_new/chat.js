// script.js
document.getElementById('send-button').addEventListener('click', function() {
    const chatWindow = document.getElementById('chat-window');
    const chatInput = document.getElementById('chat-input');
    const message = chatInput.value.trim();

    if (message !== "") {
        const messageElement = document.createElement('div');
        messageElement.textContent = message;
        messageElement.classList.add('chat-message');
        chatWindow.appendChild(messageElement);
        chatInput.value = '';
        chatWindow.scrollTop = chatWindow.scrollHeight;

        const messageElement_a = document.createElement('div');
        messageElement_a.textContent = "ただいまチャット機能はメンテナンス中です．";
        messageElement_a.classList.add('chat-message_a');
        chatWindow.appendChild(messageElement_a);
        chatInput.value = '';
        chatWindow.scrollTop = chatWindow.scrollHeight;
    }
});

document.getElementById('chat-input').addEventListener('keypress', function(event) {
    if (event.key === 'Enter') {
        document.getElementById('send-button').click();
    }
});

//レンダリング

function loadScript(url) {
    var script = document.createElement('script');
    script.src = url;
    script.async = true; // 非同期で読み込む
    document.head.appendChild(script);
}

// JavaScriptでスタイルシートを非同期で読み込む関数
function loadStylesheet(url) {
    var link = document.createElement('link');
    link.rel = 'stylesheet';
    link.href = url;
    document.head.appendChild(link);
}

// 例: スクリプトを非同期で読み込む
loadScript('example.js');

// 例: スタイルシートを非同期で読み込む
loadStylesheet('example.css');
