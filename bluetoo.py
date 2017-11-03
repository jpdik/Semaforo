import bluetooth

BD_ADDR = "20:16:10:20:67:58" #itade address
BD_PORTA = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((BD_ADDR, BD_PORTA))
print 'Bluetooth Connected'

sock.send('N')

dados = ''

while True:
	dados += sock.recv(1024)
	if dados.find('\n') != -1:
		break

print dados