#coding utf-8
import bluetooth

BD_ADDR = "20:16:10:20:67:58" #itade address
#BD_ADDR = "20:16:10:20:67:14" #itade address
BD_PORTA = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((BD_ADDR, BD_PORTA))
print 'Bluetooth Connected'
sock.settimeout(5.0)

while True:
	raw_input("algo: ")

	sock.send('D') 			# Requisita dados
	sock.recv(1) 				# Aguarda ok
	sock.send('4\n'); 					# Requisita lista de semaforos ao arduino
	dados = ''

	while True:
		dados += sock.recv(1)
		if dados.find('\n') != -1:
			break

	print dados 				# Semaforos no formato lista. Ex: [1, 2, 3, 4]