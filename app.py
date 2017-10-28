# coding: utf-8
import json
import os
import sys
import requests
import socket
import threading

from flask import Flask, Response, render_template, request

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
    return json.dumps({'erro': 'Método inválido'})

if __name__ == "__main__":
  app.run(debug=True)
