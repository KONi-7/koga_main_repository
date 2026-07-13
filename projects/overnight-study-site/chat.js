const chatWindow = document.getElementById('chat-window');
const chatInput = document.getElementById('chat-input');
const sendButton = document.getElementById('send-button');

function appendMessage(text, className) {
    const messageElement = document.createElement('div');
    messageElement.textContent = text;
    messageElement.classList.add(className);
    chatWindow.appendChild(messageElement);
    chatWindow.scrollTop = chatWindow.scrollHeight;
}

function sendMessage() {
    const message = chatInput.value.trim();

    if (message === '') {
        return;
    }

    appendMessage(message, 'chat-message');
    appendMessage('これは画面表示を確認するデモです。メッセージは保存・送信されません。', 'chat-message_a');
    chatInput.value = '';
    chatInput.focus();
}

sendButton.addEventListener('click', sendMessage);
chatInput.addEventListener('keydown', (event) => {
    if (event.key === 'Enter' && !event.isComposing) {
        event.preventDefault();
        sendMessage();
    }
});
