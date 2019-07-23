import bluetooth
import unicodedata
from flask import (
    Flask,
    render_template,
    request,
    jsonify
)


app = Flask(__name__, static_url_path='/static')
display = ('98:D3:32:70:D8:97', 1)

def strip_accents(s):
    return ''.join(c for c in unicodedata.normalize('NFD', s)
                     if unicodedata.category(c) != 'Mn')


@app.route('/')
def title_page():
    return render_template('index.html')


@app.route('/message', methods=['GET', 'POST'])
def rfcomm_interface():
    response = {}
    status = 200

    s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
    try:
        s.connect(display)

        if request.method == 'GET':
            s.send('?\n')
            msg = s.recv(4096).decode('ascii')
            # Len 1 znak print(msg)

        elif request.method == 'POST':
            msg = strip_accents(request.json['message'])
            glyphs = request.json['glyphs']

            for addr, char in glyphs.items():
                s.send('C {} {}\n'.format(addr, char))
            s.send('M {}\n'.format(msg))

        return jsonify({'message': msg}), 200

    except bluetooth.btcommon.BluetoothError as err:
        err = eval(str(err))
        return jsonify({'error': err[1]}), 503

    finally:
        s.close()
