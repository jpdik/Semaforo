# coding: utf-8
import bluetooth
import json
from flask import Flask, Response, render_template, request

BD_ADDR = "20:16:10:20:67:58" #itade address
BD_PORTA = 1

app = Flask(__name__)

@app.after_request
def add_header(response):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also to cache the rendered page for 10 minutes.
    """
    response.headers['X-UA-Compatible'] = 'IE=Edge,chrome=1'
    response.headers['Cache-Control'] = 'public, max-age=0'
    return response

@app.route('/', methods=['GET', 'POST', 'PUT', 'DELETE'])
def api():
  if request.method == 'GET':
    return render_template('index.html') 
  else:
    return json.dumps({'erro': 'Metodo invalido'})

@app.route('/semaforos', methods=['GET'])
def get_semaforos():
	try:
		return json.dumps(comando('N'));
	except bluetooth.btcommon.BluetoothError:
		return json.dumps({'erro': 'Comunicacao'})

@app.route('/criar_regra', methods=['POST'])
def criar_regra():
	try:
		return json.dumps(comando('N'));
	except bluetooth.btcommon.BluetoothError:
		return json.dumps({'erro': 'Comunicacao'})

#Bluetooh
def comando(command):
	sock = bluetooth.BluetoothSocket( bluetooth.RFCOMM )

	sock.connect((BD_ADDR, BD_PORTA))

	sock.send(command)

	dados = ''

	while True:
		dados += sock.recv(1024)
		if dados.find('\n') != -1:
			break

	try:
		return dados.replace('\n', '').replace('\r', '').encode('utf-8') 
	except UnicodeDecodeError:
		raise bluetooth.btcommon.BluetoothError

if __name__ == "__main__":
  app.run(debug=True)
