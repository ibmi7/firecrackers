import paho.mqtt.subscribe as subscribe

m = subscribe.simple(topics=['#'], hostname="thethings.example.com", port=1883, auth={'username':"app1",'password':"NNSXS.VEEBURF3KR77ZR.."}, msg_count=2)
for a in m:
    print(a.topic)
    print(a.payload)