# coding: utf-8
import bluetooth
import json
from flask import Flask, Response, render_template, request
from threading import Thread
import os.path as path
import time

BD_ADDR = "20:16:10:20:67:58"  # itade address
BD_PORTA = 1

FILE = 'data/semaforo.json'

app = Flask(__name__)
th = Thread()

finished = 0
tentativas = 0
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

semaforos = []

dados = {}

# Cria/modifica/carrega/remove do arquivo json(banco de dados)
def atualizar_file(data):
    global dados
    dados['grupos'][unicode(str(dados['maxid']), "utf-8")] = data
    dados['maxid'] += 1
    with open(FILE, 'w') as f:
        json.dump(dados, f, sort_keys=True, indent=4)


def deletar_from_file(data):
    global dados
    for i in dados['grupos']:
        if int(i) == int(data):
            dados['grupos'].pop(i)
            break
    with open(FILE, 'w') as f:
        json.dump(dados, f, sort_keys=True, indent=4)


def carregar_file():
    global dados
    if(path.exists(FILE)):
        dados = json.load(open(FILE, 'r'))

        print " * File:({}) carregado!".format(FILE)
    else:
        dados['grupos'] = {}
        dados['maxid'] = 1
        with open(FILE, 'w') as f:
            json.dump(dados, f, sort_keys=True, indent=4)
        print " * File:({}) criado com sucesso!".format(FILE)

#Impede cache
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

@app.route("/", methods=['GET'])
def load():
    global th
    global finished

    #Carrega a conexão com o bluettoh
    if finished == 0:
        th = Thread(target=init_b, args=())
        th.start()
        return render_template('loading.html')
    #Conexão com bluetooth bem sucedida? carrega a página principal com os semáforos
    elif finished == 1:
        global dados
        if request.method == 'GET':
            return render_template('index.html', semaforos=semaforos_ID(0), regras=dados['grupos'], dependencias=semaforos_ID(1))

    #Problema com a conexão do bluetooth? reinicia para o recarregamento dele, exibindo uma página de erro antes
    else:
        finished = 0
        return render_template('erro.html')


def init_b():
    global semaforos
    global finished
    try:
        bluetooth_conf()
    except:
        finished = 2


@app.route('/status')
def thread_status():
    global tentativas
    global finished
    if tentativas < 8:
        tentativas += 1
    else:
        tentativas = 0
        finished = 2
    return json.dumps(dict(status=('finished' if finished == 1 or finished == 2 else 'running')))


@app.route('/regras', methods=['GET'])
def get_regras():
    return json.dumps(dados)

@app.route('/regras/status', methods=['POST'])
def get_regra_status():
    global dados
    data = {}
    info = json.loads(request.data)
    for i in info['ids']:
        data[i] = {}
        for j in dados['grupos'][i]['semaforos']:
            try:
                data[i][j] = comando('D', j)
            except:
                data[i][j] = -1

    return json.dumps(data)

@app.route('/regra/criar', methods=['POST'])
def criar_regra():
    data = json.loads(request.data)
    atualizar_file(data)
    configura_semaforos()
    mudaEstado_semaforos(data, 1)
    reinicia_semaforos(data)
    return json.dumps({'status': 1})

@app.route('/regra/manutencao', methods=['POST'])
def manutencao_regra():
    data = json.loads(request.data)
    mudaEstado_semaforos(dados['grupos'][data['id']], 0)
    return json.dumps({'status': 1, 'id': data['id']})

@app.route('/regra/reiniciar', methods=['POST'])
def reiniciar_regra():
    data = json.loads(request.data)
    mudaEstado_semaforos(dados['grupos'][data['id']], 1)
    reinicia_semaforos(dados['grupos'][data['id']])
    return json.dumps({'status': 1, 'id': data['id']})

@app.route('/regra/reconfigurar', methods=['POST'])
def reconfigurar_regras():
    configura_semaforos()
    return json.dumps({'status': 1})

def configura_semaforos():
    for i in calcula_tempos():
        comando('C', i)

def mudaEstado_semaforos(data, estado):
    print "ok"
    global dados
    if data == "":
        for i in dados['grupos']:
            ids = ''
            for j in dados['grupos'][i]['semaforos']:
                ids += j+'-'+str(estado)+'|'
            comando('T', ids[:-1])
            print ids
    else:
        ids = ''
        for j in data['semaforos']:
            ids += j+'-'+str(estado)+'|'
        comando('T', ids[:-1])
    
def reinicia_semaforos(data=""):
    global dados
    if data == "":
        for i in dados['grupos']:
            ids = ''
            primeiro = 1
            for j in dados['grupos'][i]['semaforos']:
                ids += j+'-'+str(primeiro)+'|'
                if primeiro == 1:
                    primeiro = 0
            comando('R', ids[:-1])
            print ids
    else:
        ids = ''
        primeiro = 1
        for j in data['semaforos']:
            ids += j+'-'+str(primeiro)+'|'
            if primeiro == 1:
                primeiro = 0
        comando('R', ids[:-1])


def config_init():
    configura_semaforos()
    mudaEstado_semaforos("", 1)
    reinicia_semaforos("")   

@app.route('/regra/deletar', methods=['DELETE'])
def deletar_regra():
    data = json.loads(request.data)
    deletar_from_file(data['id'])
    return json.dumps({'status': 1, 'id': data['id']})

# semaforos


def semaforos_ID(usados):
    global semaforos
    global dados

    semaforos = json.loads(comando('N'))

    sem = []
    todos = []

    for i in dados['grupos']:
        for j in dados['grupos'][i]['semaforos']:
            todos += map(int, j)

    if usados == 1:
        sem += [x for x in semaforos if x in todos and x not in sem]
    else:
        sem += [x for x in semaforos if x not in todos and x not in sem]
    return sem

# Bluetooh
def bluetooth_conf():
    global sock, finished
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    sock.connect((BD_ADDR, BD_PORTA))
    sock.settimeout(5.0)
    print 'Bluetooth Connected'

    #th = Thread(target=config_init, args=())
    #th.start()

    finished = 1

    try:
        while True:
            continue
    except:
        return

def comando(comando, dados=''):
    global sock
    data = ""
    info = ""
    while True:
        try:
            if comando == 'N':
                sock.send(comando)
                while True:
                    info += sock.recv(1)
                    if info.find('\n') != -1:
                        break
            elif comando == 'D':
                sock.send(comando)
                sock.recv(1)
                sock.send(dados + '\n')
                while True:
                    info += sock.recv(1)
                    if info.find('\n') != -1:
                        break
            else:
                sock.send(comando)
                sock.recv(1)
                sock.send(dados + '\n')
                return
            break
        except bluetooth.btcommon.BluetoothError:
            continue
    

    try:
        data = info.replace('\n', '').replace(
                '\r', '').encode('utf-8')
    except UnicodeDecodeError:
        raise bluetooth.btcommon.BluetoothError
    return data


def calcula_tempos():
    global dados
    lista_dep = []  # lista de ids grupos que tem dependência
    lista_sem_dep = []  # lista de ids grupos que não tem dependência

    semaforos = {}  # objeto que contém a informção de qual grupo pertence um semáforo

    # Percorre cara grupo gerando um objeto que contem o id de cada grupo por semáforo e
    # o tempo total do grupo
    for id_grupo in dados['grupos']:
        dep = False
        dep_de = '0'
        dep_para = '0'
        tempo_total = 0

        # cada semaforo
        for id_semaforo in dados['grupos'][id_grupo]['semaforos']:
            semaforos.update({id_semaforo: id_grupo})
            if dados['grupos'][id_grupo]['semaforos'][id_semaforo]['dependencia'] != '0':
                dep = True
                dep_de = id_semaforo
                dep_para = dados['grupos'][id_grupo]['semaforos'][id_semaforo]['dependencia']
            else:
                tempo_total += float(dados['grupos'][id_grupo]
                                     ['semaforos'][id_semaforo]['tempo_aberto'])

        if dep == True:
            tempo_total = 0
            lista_dep.append(
                {'id_grupo': id_grupo, 'dep_de': dep_de, 'dep_para': dep_para})
        else:
            lista_sem_dep.append(id_grupo)

        dados['grupos'][id_grupo].update({'tempo_total': tempo_total})

    saida = ''

    # Calcula o tempo de cada grupo que é independente de outros
    while lista_sem_dep:
        id_grupo = lista_sem_dep.pop()

        for id_semaforo in dados['grupos'][id_grupo]['semaforos']:
            dados['grupos'][id_grupo]['semaforos'][id_semaforo].update({'tempo_fechado':
            dados['grupos'][id_grupo]['semaforos'][id_semaforo]['tempo_fechado'] if len(dados['grupos'][id_grupo]['semaforos']) == 1 else dados['grupos'][id_grupo]['tempo_total'] - float(dados['grupos'][id_grupo]['semaforos'][id_semaforo]['tempo_aberto'])})

    # Calcula o tempo de cada grupo que é dependente de um outro
    while lista_dep:
        id_grupo = lista_dep.pop()
        tempo_dep = float(dados['grupos'][semaforos[id_grupo['dep_para']]]
                          ['semaforos'][id_grupo['dep_para']]['tempo_aberto'])
        tempo_total = dados['grupos'][semaforos[id_grupo['dep_para']]
                                      ]['tempo_total'] - tempo_dep
        quantidade = len(
            dados['grupos'][id_grupo['id_grupo']]['semaforos']) - 1

        for id_semaforo in dados['grupos'][id_grupo['id_grupo']]['semaforos']:
            tempo_fechado = 0
            tempo_aberto = 0
            if id_semaforo == id_grupo['dep_de']:
                tempo_aberto = tempo_dep
                tempo_fechado = tempo_total
            else:
                tempo_aberto = tempo_total / quantidade
                if quantidade == 1:
                    tempo_fechado = tempo_dep
                else:
                    tempo_fechado = (
                        tempo_total / (quantidade - 1)) + tempo_dep

            dados['grupos'][id_grupo['id_grupo']]['semaforos'][id_semaforo].update(
                {'tempo_aberto': tempo_aberto, 'tempo_fechado': tempo_fechado})

    ret = []  # id_sem;aberto,fechado

    for id_grupo in dados['grupos']:
        for id_semaforo in dados['grupos'][id_grupo]['semaforos']:
            ret.append('{};{},{}\n'.format(id_semaforo, int(dados['grupos'][id_grupo]['semaforos'][id_semaforo]['tempo_aberto']), int(
                dados['grupos'][id_grupo]['semaforos'][id_semaforo]['tempo_fechado'])))

    return ret


if __name__ == "__main__":
    carregar_file()
    app.run(debug=True)
