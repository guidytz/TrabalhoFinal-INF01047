# Trabalho Final - INF01047 - Jogo Sokoban

## v 0.0
### Versão apenas de testes inicial
#### Funcionalidades por enquanto:
 * Renderização do globo como o Lab 5
 * Renderização do plano como o Lab 5
 * Renderização de uma mão que acompanha a câmera

## v 0.1
### Versão que contém a cena quando o jogo termina
#### Funcionalidades adicionadas
 * Aprensenta trofeis quando o jogo termina
 * Troféu maior fica girando com o tempo
 * Troféis menores se movimentam por uma curva de Bezier ao redor do trofeu maior
 * A atualização da cena é toda feita baseada na diferença de tempo usando a função glfwGetTime()

## v 0.5
### Versão que implementa colisões
#### Funcionalidades adicionadas
 * Renderização do mapa e uma caixa
 * Fixação vertical do personagem
 * Colisões do player com as paredes do mapa (precisa de algumas melhorias)
 * Colisão da mão com a caixa para que possa ser empurrada (ainda precisa ajustar a bounding sphere da mão)
 * Colisão da caixa com as paredes do mapa
