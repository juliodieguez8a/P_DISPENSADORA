# -*- coding: utf-8 -*-
"""
Created on Mon May 29 15:29:52 2017

@author: Boggdan Barrientos
"""
# Simple example of using general timer objects. This is used to update
# the time placed in the title of the figure.
import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime
from matplotlib.widgets import Button, Slider, RadioButtons
import serial

valor = 0
cont = 0
xGraph = []
yLec = []
yEnv = []
giro_derecha = 0
env = 0
var = 0
moneda = ""
temperature = ""

#ser = serial.Serial('COM4', baudrate = 300, timeout=1,bytesize = 8, stopbits = serial.STOPBITS_ONE)               
#ser.close()
    
def vel_Lectura(velocidad):
    a = int(ord(velocidad))
    a = a*0.04
    return ( a )
    
def enviar_Valor(data):
    #ser.open()
    valor = chr(data)
    #ser.write(valor)
    #ser.close()
    return ( valor )

#==============================================================================
def update_title(axes):
    #ser.open()
    global cont, valor
    xGraph.append(cont)
    yLec.append(vel_Lectura(ser.readline(1)))   
    yEnv.append(valor)
    print yLec
    ax.cla()
    axes.figure.canvas.draw()
    
    ax.plot(xGraph, yLec)
    ax.plot(xGraph, yEnv)
    ax.set_ylabel('w(Rev/s)')
    ax.set_xlabel('tiempo(s)')
    ax.set_xlim([cont-10,cont+10])

    ax.set_ylim([0,20])    
    
     
    ax.grid()
    plt.draw()    
    #ser.close()
    cont += 0.1
    
#==============================================================================
class Index(object):
    
    def entregar(self, event):
        
        if (compra == 1):
            enviar_Valor(5)
            print ("Se ha despachado un Snack")
        if (compra == 0):
            enviar_Valor(6)
            print ("Se ha despachado una Bebida")
        
    def monedas(self, event):
        global moneda,temperature
        enviar_Valor(1)
        #ser.open()
        #temp = str( ser.readline(1) )
        moneda = "Q 1.50"
        textCoin = plt.text(0.3,10,moneda)
        textTemperature = plt.text(0.2,6.2,temperature)
        #ser.close()
        print ("Desplegando monedas actuales")    
        
    def temperatura(self, event):
        global temperature,moneda
        enviar_Valor(2)
        #ser.open()
        #temp = ( ser.readline(1) )
        temperature = "   25.0 C"
        textCoin = plt.text(0.3,10,moneda)
        textTemperature = plt.text(0.2,6.2,temperature)
        #ser.close()
        print ("Desplegando temperatura")    
        
    def iluminacion(self, event):
        enviar_Valor(3)
        print ("Encendiendo iluminacion")    

    def desplegar(self, event):
        enviar_Valor(4)
        print ("Mostrando mensaje en LCD")  
        
    def status(self,event):
        global compra
        radioValue = str(butRadio.value_selected)       
        if (radioValue == "Snack") == True :
            compra = 1
        else:
            compra = 0
        
        
#==============================================================================

fig, ax = plt.subplots(figsize=(12, 10))
plt.subplots_adjust(bottom = 0.2,left=0.25)
plt.title("Graficas") 
plt.ylim([0,20])
plt.xlim([0,+100])
plt.grid()

callback = Index()
axRadio = plt.axes([0.55, 0.005, 0.10, 0.1])
butRadio = RadioButtons(axRadio, ('Bebida','Snack'))
butRadio.on_clicked(callback.status)
axlec = plt.axes([0.7, 0.005, 0.15, 0.075])
blec = Button(axlec, 'ENTER')
blec.on_clicked(callback.entregar)
axMonedas = plt.axes([0.05, 0.8, 0.15, 0.05])
bmoney = Button(axMonedas, 'Estado de compra')
bmoney.on_clicked(callback.monedas)
axTemperatura = plt.axes([0.05, 0.625, 0.15, 0.05])
btemp = Button(axTemperatura, 'Temperatura')
btemp.on_clicked(callback.temperatura)
axIluminacion = plt.axes([0.05, 0.45, 0.15, 0.05])
bluz = Button(axIluminacion, 'Iluminacion')
bluz.on_clicked(callback.iluminacion)    
axLCD = plt.axes([0.05, 0.25, 0.15, 0.05])
blcd = Button(axLCD, 'Display')
blcd.on_clicked(callback.desplegar)
textCoin = plt.text(0.3,10,moneda)
textTemperature = plt.text(0.2,6.2,temperature)
plt.show()



#==============================================================================

timer = fig.canvas.new_timer(interval=100)
timer.add_callback(update_title, ax)
timer.start()


