#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright (c) 2016 Roger Light <roger@atchoo.org>
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Distribution License v1.0
# which accompanies this distribution.
#
# The Eclipse Distribution License is available at
#   http://www.eclipse.org/org/documents/edl-v10.php.
#
# Contributors:
#    Roger Light - initial implementation

# This shows an example of using the subscribe.simple helper function.

import paho.mqtt.subscribe as subscribe

topics = ['#']

m = subscribe.simple(topics=["v3/beekeeping@ttn/devices/beekeeping/up"], hostname="eu1.cloud.thethings.network", port=1883, auth={'username':"beekeeping@ttn",'password':"NNSXS.XD3ULAGQOUKUFY5AOSMWDTA2ASOHEINI5NISUJY.26XSRTNGV5TLETKXTAD5MI4TDWNSYZL5GB2SOUNF2QJND4RGH4CA"}, msg_count=2)
for a in m:
    print(a.topic)
    print(a.payload)