from flask import Flask, render_template

app = Flask(__name__)
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0


@app.route('/')
def index():
    return render_template('index.html')


def main():
    app.run(debug=True, host='0.0.0.0', port=8080)


if __name__ == '__main__':
    main()
