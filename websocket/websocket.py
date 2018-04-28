import asyncio
import websockets
import numpy as np
import netifaces as ni
#from sceptre_training_service import train,getMetaModels
#from sceptre_testing_service import test
#from imu_data_importer import on_mpu, write_arrays_to_file
##### TODO : move class to HomeX_Core_Python
class Sceptre:
    nick =""
    ip= ""
    websocket = None
    outfile=open(nick+".csv","w");
    async def notify(self,message):
        await self.websocket.send(message)

    def writeData(self,data):
        print(data)
        print (self.nick+"recived datastream:"+";".join(data))
        self.outfile.write(";".join(data)+"\n")
        self.outfile.flush()

    def closeFile(self):
        outfile.close()

 
 
class Server:
    sceptres = []
    UIs = []
    def startServer(self,netAdapterName):
        print("[GestureBaba] Starting server...")
        start_server = websockets.serve(self.mainController,self.getIPAddress(netAdapterName),81)
        print("[GestureBaba] Server callback registered.")
        asyncio.get_event_loop().run_until_complete(start_server)
        asyncio.get_event_loop().run_forever()
 
    def getIPAddress(self,ifname):
        ni.ifaddresses(ifname)
        ip = ni.ifaddresses(ifname)[ni.AF_INET][0]['addr']
        print("[GestureBaba] Server IP : " + ip)
        return ip
    async def mainController(self,websocket,path):
        print("[] Server Started!!")
        while(True):
            text = await websocket.recv()
            message = [x.strip() for x in text.split(';')]
            print("[GestureBaba] : RECV : " + text)
            header = message[0]
            if(header=="sconnected"):
                sceptre = Sceptre()
                sceptre.ip = message[1]
                sceptre.nick = message[2]
                sceptre.websocket = websocket
                # remove
                #await sceptre.notify("startx")
                index=0
                for existingSceptre in self.sceptres:
                    if(existingSceptre.nick==sceptre.nick):
                        print("Found existing sceptre with nick : " + sceptre.nick + ". Updating entry")
                        self.sceptres[index] = sceptre
                        break
                    index = index + 1
                if(index == len(self.sceptres)):
                    print("Found new sceptre : " + sceptre.nick)
                    self.sceptres.append(sceptre)
                print('sceptre connected')
            #request : uiconnected;{IP_Address};{UI_nick}
            elif (header=="uiconnected"):
                ui = UI_haha()
                ui.websocket = websocket
                ui.ip = message[1]
                ui.nick = message[2]
                index = 0
                for existingUI in self.UIs:
                    if(existingUI.nick == ui.nick):
                        print("Found existing UI with nick : " + ui.nick + ". Updating entry")
                        self.UIs[index] = ui
                        break
                    index = index + 1
                if(index == len(self.UIs)):
                    print("Found new UI : " + ui.nick)
                    self.UIs.append(ui)
                print('UI connected')
            #request : get_available_sceptres;{UI_nick}
            elif (header=="get_available_sceptres"):
                available_sceptres_nick = [x.nick for x in self.sceptres]
                response_string = ','.join(available_sceptres_nick)
                UI_nick= message[1]
                UI = self.findUI(UI_nick)
                if(UI!=None):
                    await UI.notify("get_available_sceptres;" + response_string)
            #request : bind-sceptre;{sceptre_nick};{UI_nick}
            elif(header=="bind-sceptre"):
                UI_nick = message[2]
                sceptre_nick = message[1]
                UI = self.findUI(UI_nick)
                sceptre = self.findSceptre(sceptre_nick)
                #print("[GestureBaba] Sceptre : " + sceptre + " UI : " +UI )
                if(UI!=None and sceptre!=None):
                    UI.sceptre = sceptre
                    sceptre.UI = UI
                    await UI.notify("bind-sceptre;success")
                else:
                    await websocket.send("bind-sceptre;failed. Either requested sceptre or this UI is not registered")
            #request : startx;{UI_nick}
            elif(header=="startx"):
                UI_nick = message[1]
                UI = self.findUI(UI_nick)
                if(UI == None):
                    await websocket.send("startx;error;UI_not_registered")
                    continue
                elif(UI.sceptre == None):
                    await UI.notify("startx;error;no sceptre binding present")
                    continue
                # UI is not None and has a bound sceptre
                await UI.sceptre.notify("startx")
            #request : stopx;{UI_nick}
            elif(header=="stopx"):
                UI_nick = message[1]
                UI = self.findUI(UI_nick)
                if(UI == None):
                    await websocket.send("stopx;error;UI_not_registered")
                    continue
                elif(UI.sceptre == None):
                    await UI.notify("stopx;error;no sceptre binding present")
                    continue
                # UI is not None and has a bound sceptre
                await UI.sceptre.notify("stopx")
            #request : mpu;{sceptre_nick};ax,ay,az,.......,13 values of feature vector
            #response: test;{detected_gesture}
            elif(header=="data"):
                print(message)
                sceptre_nick = message[1]
                sceptre = self.findSceptre(sceptre_nick)
                if(sceptre==None):
                    print("[GestureBaba] : Sceptre with nick : " + sceptre_nick + " was not registered")
                    continue
                values = message[2:]
                print(values)
                sceptre.writeData(values)

 
    def findSceptre(self,nick):
        for sceptre in self.sceptres:
            if(sceptre.nick==nick):
                return sceptre
        return None
    def findUI(self,nick):
        for UI in self.UIs:
            if(UI.nick==nick):
                return UI
        return None
if __name__ == "__main__":
    server = Server()
    start_server = server.startServer("wlp2s0")
