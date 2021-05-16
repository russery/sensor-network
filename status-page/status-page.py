from flask import Flask, render_template

app = Flask(__name__)
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/temperature')
def temperature():
    return {'upstairs': 73.0, 'downstairs': 74.0 }

@app.route('/humidity')
def humidity():
    return {'upstairs': 30.0, 'downstairs': 32.0 }

def main():
    app.run(debug=True, host='0.0.0.0')

if __name__ == '__main__':
    main()