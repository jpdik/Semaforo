# coding: utf-8
import bluetooth
import json
from flask import Flask, Response, render_template, request

import os.path as path

BD_ADDR = "20:16:10:20:67:58" #itade address
BD_PORTA = 1

FILE = 'data/semaforo.json'

app = Flask(__name__)

dados = {}

def atualizar_file(data):
	data['id'] = dados['maxid']
	dados['maxid'] += 1
	dados['semaforos'].append(data)
	with open(FILE, 'w') as f:
		json.dump(dados, f)

def carregar_file():
	global dados
	if(path.exists(FILE)):
		dados = json.load(open(FILE, 'r'))

		print " * File:({}) carregado!".format(FILE)
	else:
		dados['semaforos'] = []
		dados['maxid'] = 1
		with open(FILE, 'w') as f:
			json.dump(dados, f)
		print " * File:({}) criado com sucesso!".format(FILE)
 
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
		return json.dumps({'status': 1, 'semaforos': json.loads(comando('N'))});
	except bluetooth.btcommon.BluetoothError:
		return json.dumps({'status': 0, 'erro': 'Comunicacao'})

@app.route('/criar_regra', methods=['POST'])
def criar_regra():
	data = json.loads(request.data)
	atualizar_file(data)
	return json.dumps({'status': 1})

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
  carregar_file()
  app.run(debug=True)