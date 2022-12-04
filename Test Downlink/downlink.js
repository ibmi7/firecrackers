
function decimalToHex(d, padding) {
    var hex = Number(d).toString(16);
    padding = typeof (padding) === "undefined" || padding === null ? padding = 2 : padding;

    while (hex.length < padding) {
        hex = "0" + hex;
    }

    return hex;
}

function hexToBase64(hexstring) {
    return btoa(hexstring.match(/\w{2}/g).map(function(a) {
        return String.fromCharCode(parseInt(a, 16));
    }).join(""));
}

function sendInterval() {

        var interval = document.getElementById("interval").value;
        var intervalHex = decimalToHex(interval,2);
        var intervalBase64 = hexToBase64(intervalHex);
        document.getElementById("conversion").innerHTML = intervalBase64;
        const person = { userName: 'beekeeping@ttn', password: 'NNSXS.XD3ULAGQOUKUFY5AOSMWDTA2ASOHEINI5NISUJY.26XSRTNGV5TLETKXTAD5MI4TDWNSYZL5GB2SOUNF2QJND4RGH4CA' };
        // connection option
        const options = {
            clean: true, // retain session
            protocolId: 'MQTT',
            protocolVersion: 4,
            connectTimeout: 30000, // Timeout period increased to 30 seconds
            // Authentication information
            clientId: 'mqttjs_' + Math.floor(Math.random() * 10000),
            username: 'beekeeping@ttn',
            password: 'NNSXS.XD3ULAGQOUKUFY5AOSMWDTA2ASOHEINI5NISUJY.26XSRTNGV5TLETKXTAD5MI4TDWNSYZL5GB2SOUNF2QJND4RGH4CA',
        }

        // Connect string, and specify the connection method by the protocol
        // ws Unencrypted WebSocket connection
        // wss Encrypted WebSocket connection
        // mqtt Unencrypted TCP connection
        // mqtts Encrypted TCP connection
        // wxs WeChat applet connection
        // alis Alipay applet connection
        const connectUrl = 'ws://eu1.cloud.thethings.network:1883';
        const client = mqtt.connect(connectUrl,options);
        //actually subscribe to something on a sucessfull connection
        client.on('connect', (connack) => {
            client.subscribe('$SYS/#')
        })
        
        client.on('reconnect', (error) => {
            console.log('reconnecting:', error)
        })
        
        client.on('error', (error) => {
            console.log('Connection failed:', error)
        })
        
        client.on('message', (topic, message) => {
            console.log('receive message：', topic, message.toString())
        })
}

function onConnect() {
    console.log("Connected");
    Client.subscribe("v3/beekeeping@ttn/devices/beekeeping/up");
    down = '{"downlinks":[{"f_port": 15,"frm_payload":"' + intervalBase64 + '","priority": "NORMAL"}]}'
    Client.publish("v3/beekeeping@ttn/devices/beekeeping/down/push",down);
}