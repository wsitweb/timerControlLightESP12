function toggleLED() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/toggle', true);
    xhr.send();
    xhr.onload = function() {
        if (xhr.status == 200) {
            const button = document.getElementById('toggleButton');
            button.innerText = button.innerText === 'Включить' ? 'Выключить' : 'Включить';
        }
    };
}

function updateTimers() {
    const onTime = document.getElementById('onTime').value;
    const offTime = document.getElementById('offTime').value;
    const statusText = document.getElementById('text');

    const xhr = new XMLHttpRequest();
    xhr.open('GET', '/updateTimers', true);
    statusText.innerText = 'Open';

    xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    statusText.innerText = 'set Request Header';

    xhr.send('onTime=' + onTime + '&offTime=' + offTime);
    statusText.innerText = 'send';

    xhr.onload = function() {
        if (xhr.status == 200) {
            statusText.innerText = 'готово';
            console.log(xhr.status, xhr.responseText)
        }
    }
}
