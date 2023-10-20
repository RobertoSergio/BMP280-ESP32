#include <Wire.h>  // Biblioteca para o protocolo de cominicação I2C
#include <Adafruit_Sensor.h> 
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;  

#define TAMANHO_FILTRO 50 // Quantidade de amostras consideradas pelo filtro

int32_t leiturasAltitude[TAMANHO_FILTRO];
int32_t leiturasTemperatura[TAMANHO_FILTRO];
int indice = 0;

void setup() {
  delay(10000);  // Atraso para limpar Monitor Serial e preparar teste
  Serial.begin(115200);  // Iniciando porta Serial

  if (!bmp.begin(0x76)) {
    Serial.println("Não foi possível encontrar um sensor BMP280.");
    while (1);
  }

  // Configurar o filtro
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Modo de medição
                  Adafruit_BMP280::SAMPLING_X2,     // Taxa de amostragem de temperatura
                  Adafruit_BMP280::SAMPLING_X16,    // Taxa de amostragem de pressão
                  Adafruit_BMP280::FILTER_X16,      // Filtro
                  Adafruit_BMP280::STANDBY_MS_500); // Tempo de espera
}

void loop() {
  // Ler os dados do sensor
  int32_t altitude = bmp.readAltitude();
  int32_t temperatura = bmp.readTemperature();

  // Começar o uso do Filtro média movel
  leiturasAltitude[indice] =  calcularMedia(altitude);
  leiturasTemperatura[indice] = calcularMedia(temperatura);

  // Printar os Dados
  Serial.print("Altitude (com filtro): ");
  Serial.print(leiturasAltitude[indice] / 100.0, 2); 
  Serial.println(" Metros");

  Serial.print("Temperatura (com filtro): ");
  Serial.print(leiturasTemperatura[indice] / 100.0, 2);
  Serial.println(" °C");

  // Atualizar o índice
  indice = (indice + 1) % TAMANHO_FILTRO;

  delay(1000);
}

/*
  Função que implementa a Média Móvel.
  Média Móvel: é uma tecnica de filtragem de sinal que dinimui o ruído existente nos dados
  brutos do sensor. Foi implementada a Média Móvel Simples a qual utiliza a média das últimas
  N medidas para calcular o valor real mais provével.
*/

int32_t calcularMedia(int32_t novaLeitura) {
  static int i = 0, j = TAMANHO_FILTRO - 1;
  static int32_t aux[TAMANHO_FILTRO], soma = 0;

  soma = soma + novaLeitura - aux[i];
  aux[j] = novaLeitura;
  j++; j %= TAMANHO_FILTRO;
  i++; i %= TAMANHO_FILTRO;
  soma = soma / (j - i + 1);

  return soma;
}
