const http = require('http');
const five = require('johnny-five');
const Board = require('firmata');

const board = new Board('/dev/ttyUSB0'); // Ajuste para a porta correta do seu NodeMCU

board.on('ready', () => {
  const bmp280 = new five.BMP280({
    controller: 'BMP280',
    i2cAddress: 0x76 // ou 0x77, dependendo do seu sensor
  });

  // Cria o servidor HTTP
  const server = http.createServer((req, res) => {
    if (req.method === 'GET' && req.url === '/') {
      bmp280.read((err, data) => {
        if (err) {
          console.error(err);
          res.writeHead(500, { 'Content-Type': 'text/plain' });
          res.end('Error reading from sensor');
          return;
        }

        const { temperature, pressure, altitude } = data;

        // HTML básico para exibir os dados do sensor
        res.writeHead(200, { 'Content-Type': 'text/html' });
        res.end(`
          <!DOCTYPE html>
          <html>
          <head>
            <title>Dados do Sensor BMP280</title>
          </head>
          <body>
            <h1>Dados do Sensor BMP280</h1>
            <p>Temperatura: ${temperature.toFixed(2)} °C</p>
            <p>Pressão: ${pressure.toFixed(2)} hPa</p>
            <p>Altitude: ${altitude.toFixed(2)} m</p>
          </body>
          </html>
        `);
      });
    } else {
      res.writeHead(404, { 'Content-Type': 'text/plain' });
      res.end('404 Not Found');
    }
  });

  // Inicia o servidor na porta 3000
  server.listen(3000, () => {
    console.log('Servidor rodando em http://localhost:3000');
  });
});
