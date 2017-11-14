# coding: utf-8
import bluetooth
import json
from flask import Flask, Response, render_template, request

import os.path as path

BD_ADDR = "20:16:10:20:67:58"  # itade address
BD_PORTA = 1

FILE = 'data/semaforo.json'

app = Flask(__name__)

semaforos = [1, 2, 3, 4]

dados = {}


def atualizar_file(data):
    data['id'] = dados['maxid']
    dados['maxid'] += 1
    dados['grupos'].append(data)
    with open(FILE, 'w') as f:
        json.dump(dados, f, sort_keys=True, indent=4)


def deletar_from_file(data):
    for k, i in enumerate(dados['grupos']):
        if int(i['id']) == int(data):
            dados['grupos'].pop(k)
            break
    with open(FILE, 'w') as f:
        json.dump(dados, f, sort_keys=True, indent=4)


def carregar_file():
    global dados
    if(path.exists(FILE)):
        dados = json.load(open(FILE, 'r'))

        print " * File:({}) carregado!".format(FILE)
    else:
        dados['grupos'] = []
        dados['maxid'] = 1
        with open(FILE, 'w') as f:
            json.dump(dados, f, sort_keys=True, indent=4)
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

@app.route("/load")
def init():
    return render_template('loading.html')

@app.route('/', methods=['GET', 'POST', 'PUT', 'DELETE'])
def api():
    global dados
    global semaforos
    #semaforos = json.loads(comando('N'))
    if request.method == 'GET':
        return render_template('index.html', semaforos=semaforos_ID(0), regras=dados['grupos'], dependencias=semaforos_ID(1))
    else:
        return json.dumps({'erro': 'Metodo invalido'})


@app.route('/regras', methods=['GET'])
def get_regras():
    return json.dumps(dados)


@app.route('/regra/criar', methods=['POST'])
def criar_regra():
    data = json.loads(request.data)
    atualizar_file(data)
    return json.dumps({'status': 1})


@app.route('/regra/deletar', methods=['DELETE'])
def deletar_regra():
    data = json.loads(request.data)
    deletar_from_file(data['id'])
    return json.dumps({'status': 1, 'id': data['id']})

# semaforos


def semaforos_ID(usados):
    global semaforos
    sem = []
    todos = []

    for i in dados['grupos']:
        for j in i['semaforos']:
            todos += map(int, j['id'])

    if usados == 1:
        sem += [x for x in semaforos if x in todos and x not in sem]
    else:
        sem += [x for x in semaforos if x not in todos and x not in sem]
    return sem

# Bluetooh


def comando(command):
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((BD_ADDR, BD_PORTA))
    print 'Bluetooth Connected'

    sock.send(command)

    dados = ''

    while True:
        dados += sock.recv(1)
        if dados.find('\n') != -1:
            break

    try:
        return dados.replace('\n', '').replace('\r', '').encode('utf-8')
    except UnicodeDecodeError:
        raise bluetooth.btcommon.BluetoothError


if __name__ == "__main__":
    carregar_file()
    app.run(debug=True)
