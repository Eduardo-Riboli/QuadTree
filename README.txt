Passo a passo:

1. Converter a imagem para tons de cinza (seção 3.2↓), isto é, gerar uma imagem onde cada pixel possui uma 
intensidade (0 - preto, 255 - branco). Essa imagem será utilizada para decidir o momento de encerrar o algoritmo, 
através do histograma de cada região.

2. Para cada região já criada (inicialmente só teremos uma única região, abrangendo a imagem inteira):
    2.1. Calcular a cor média da região (seção 3.5↓).
    2.2. Calcular o histograma da região em tons de cinza (seção 3.3↓).
    2.3. Calcular o nível de erro da região, através do histograma (seção 3.4↓).
    2.4. Se o nível de erro for inferior ao erro mínimo especificado pelo usuário, o processo termina para essa região.
    2.5. Caso contrário, subdividir a região em 4 e repetir o algoritmo, recursivamente.