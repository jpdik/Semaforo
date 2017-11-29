#coding utf-8
import bluetooth

BD_ADDR = "20:16:10:20:67:58" #itade address
BD_PORTA = 1

sock=bluetooth.BluetoothSocket( bluetooth.RFCOMM )
sock.connect((BD_ADDR, BD_PORTA))
print 'Bluetooth Connected'

sock.send('C') 				# Configurar
sock.recv(1) 				# Aguarda ok
sock.send("1;5,10\n")		# ID;tempo aberto,tempo fechado
sock.send('C')
sock.recv(1)
sock.send('2;10,5\n')
sock.send('C')
sock.recv(1)
sock.send("3;10,5\n")
sock.send('C')
sock.recv(1)
sock.send("4;5,10\n")
sock.send('T') 				# Trocar estado
sock.recv(1) 				# Aguarda ok
sock.send('1-1|2-1|3-1|4-1\n')	# Id-estado|id-estado|...
sock.send('R') 				# Reinicia
sock.recv(1) 				# Aguarda ok
sock.send('1-1|2-0|3-0|4-1\n')	# Id-primeiro|id-naoprimeiro|...
#sock.send('D') 			# Requisita dados
#sock.recv(1) 				# Aguarda ok
#sock.send('2\n'); 			# Envia o id do qual quer receber info

sock.send('N')				# Requisita lista de semaforos ao arduino

dados = ''

while True:
	dados += sock.recv(1)
	if dados.find('\n') != -1:
		break

print dados 				# Semaforos no formato lista. Ex: [1, 2, 3, 4]