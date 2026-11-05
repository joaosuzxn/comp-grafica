# Gera atlas.ppm: 64x64 px, 4x4 células de 16x16 (N = 4).
# Célula = linha * 4 + coluna, linha contada a partir do topo da imagem.
# Usadas aqui: 0 grama topo, 1 grama lateral, 2 terra, 3 pedra, 6 folha, 7 vidro.
import random

random.seed(1)
VERDE, TERRA, PEDRA, FOLHA = (80, 150, 50), (125, 85, 55), (130, 130, 135), (50, 120, 40)
MAGENTA = (255, 0, 255)                                    # cor-chave

def ruido(cor):
    d = random.randint(-15, 15)
    return tuple(max(0, min(255, c + d)) for c in cor)

def cor_do_texel(celula, u, v):          # u, v = coluna e linha dentro da célula (0 = topo)
    if celula == 0: return ruido(VERDE)                     # grama topo
    if celula == 1: return ruido(VERDE if v < 4 else TERRA)  # faixa verde no alto
    if celula == 2: return ruido(TERRA)                     # terra
    if celula == 3: return ruido(PEDRA)                     # pedra
    if celula == 6:                                         # folha com buracos
        return MAGENTA if random.random() < 0.3 else ruido(FOLHA)
    if celula == 7:                                         # vidro com moldura
        return (170, 210, 235) if u in (0, 15) or v in (0, 15) else (205, 230, 250)
    return (40, 40, 40)                                     # células sem uso

with open("atlas.ppm", "wb") as f:
    f.write(b"P6\n64 64\n255\n")                            # cabeçalho
    for y in range(64):                                     # primeira linha = topo
        for x in range(64):
            celula = (y // 16) * 4 + (x // 16)
            f.write(bytes(cor_do_texel(celula, x % 16, y % 16)))
print("atlas.ppm gerado")
