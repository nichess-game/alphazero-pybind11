import glob
import os
import torch
import numpy as np
import time
from load_lib import load_alphazero

alphazero = load_alphazero()

THINK_TIME = 10
CPUCT = 1.25
START_TEMP = 1
END_TEMP = 0.2
TEMP_DECAY_HALF_LIFE = 10

Game = alphazero.NichessGS

bf = 0
bfc = 0


def calc_temp(turn):
    ln2 = 0.693
    ld = ln2 / TEMP_DECAY_HALF_LIFE
    temp = START_TEMP - END_TEMP
    temp *= np.exp(-ld * turn)
    temp += END_TEMP
    return temp


def eval_position(gs, agent, greedy=False):
    mcts = alphazero.MCTS(CPUCT, gs.num_players(),
                          gs.num_moves(), 0, 1.4, 0.25)
    global bf
    global bfc
    bf += np.sum(gs.valid_moves())
    bfc += 1
    start = time.time()
    sims = 0
    while time.time() - start < THINK_TIME:
        # for _ in range(200):
        leaf = mcts.find_leaf(gs)
        v, pi = agent.predict(torch.from_numpy(leaf.canonicalized()))
        v = v.cpu().numpy()
        pi = pi.cpu().numpy()
        mcts.process_result(gs, v, pi, False)
        sims += 1
    print(f'\tRan {sims} simulations in {round(time.time()-start, 3)} seconds')
    # print('Press enter for ai analysis')
    # input()
    v, pi = agent.predict(torch.from_numpy(gs.canonicalized()))
    v = v.cpu().numpy()
    pi = pi.cpu().numpy()
    print(f'\tRaw Score: {v}')
    thing = {x: round(100*pi[x], 1) for x in reversed(np.argsort(pi)[-10:])}
    print(f'\tRaw Top Probs: {thing}')
    print(f'\tRaw Best: {np.argmax(pi)}')
    print(f'\tRaw Rand: {np.random.choice(pi.shape[0], p=pi)}')

    print(f'\tMCTS Current Player WLD: {mcts.root_value()}')
    counts = mcts.counts()
    thing = {x: counts[x] for x in reversed(np.argsort(counts)[-10:])}
    print(f'\tMCTS Top Counts: {thing}')
    probs = mcts.probs(calc_temp(gs.current_turn()))
    thing = {x: round(100*probs[x], 1)
             for x in reversed(np.argsort(probs)[-10:])}
    best_move = np.argmax(probs)
    print(f'\tMCTS Top Probs: {thing}')
    print(f'\tMCTS Best: {best_move}')
    rand = np.random.choice(probs.shape[0], p=probs)
    print(f'\tMCTS Rand: {rand}')
    if greedy:
        return best_move
    else:
        return rand




from flask import Flask, request
from flask_cors import CORS
app = Flask(__name__)
CORS(app)

import neural_net
import game_runner
np.set_printoptions(precision=3, suppress=True)
nn_folder = os.path.join('data','checkpoint')
nn_file = os.path.basename(
    sorted(glob.glob(os.path.join(nn_folder, '*.pt')))[-1])
nn = neural_net.NNWrapper.load_checkpoint(Game, nn_folder, nn_file)


@app.route('/ai-action', methods = ['POST'])
def ai_action():
    encoded_position = request.form['encoded_position']
    gs = Game(encoded_position)
    print('gs made from the encoded position:')
    print(gs)
    agent_move = eval_position(gs, nn, True)
    player_action = gs.move_to_player_action(agent_move)
    return player_action

if __name__ == '__main__':
    app.run()
