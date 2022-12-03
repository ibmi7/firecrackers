import paho.mqtt.publish as publish
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
from base64 import b64encode, b64decode


class Application(tk.Tk):
    def __init__(self):
        tk.Tk.__init__(self)
        self.geometry('500x100')
        self.resizable(False, False)
        self.title('Spinbox Demo')
        # Title
        self.label1 = tk.Label(text="Choisissez l'intervalle des envoies des données (en minutes)")
        # Spinbox
        current_value = tk.StringVar(value=10)
        self.spin_box = ttk.Spinbox(
            self,
            from_=1,
            to=240,
            textvariable=current_value,
            wrap=True)

        # Button

        self.b = tk.Button(self, text="Confirm", command=self.confirmCallBack)
        self.label1.pack()
        self.spin_box.pack()
        self.b.pack()

    def confirmCallBack(self):
        value = self.spin_box.get()
        if int(value) > 240 or int(value) < 1:
            tk.messagebox.showerror(title="Mauvaise valeur", message="L'intervalle doit être compris entre 1 et 240 "
                                                                     "minutes.")
            return
        hexvalue = f"{int(value):0{2}x}"
        b64 = b64encode(bytes.fromhex(hexvalue)).decode()
        down = '{"downlinks":[{"f_port": 15,"frm_payload":"' + b64 + '","priority": "NORMAL"}]}'
        publish.single("v3/beekeeping@ttn/devices/beekeeping/down/push",
                       down,
                       hostname="eu1.cloud.thethings.network", port=1883,
                       auth={'username': "beekeeping@ttn",
                             'password': "NNSXS.XD3ULAGQOUKUFY5AOSMWDTA2ASOHEINI5NISUJY.26XSRTNGV5TLETKXTAD5MI4TDWNSYZL5GB2SOUNF2QJND4RGH4CA"})


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    essai = Application()
    essai.mainloop()
# See PyCharm help at https://www.jetbrains.com/help/pycharm/
