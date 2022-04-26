import networkx as nx
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure
from netgraph import InteractiveGraph

edges = {
    ('test.sy', 'test.ll'): 'clang -x c -emit-llvm -S\ntest.sy -o test.ll',
    ('test.ll', 'test.bc'): 'llvm-as\ntest.ll -o test.bc',
    ('test.bc', 'test.ll'): 'llvm-dis\ntest.bc -o test.ll',
    ('test.ll', 'test.s'): 'llc\ntest.ll -o test.s',
    ('test.bc', 'test.s'): 'llc\ntest.bc -o test.s',
    ('test.s', 'test.o'): 'arm-linux-gnueabihf-as\ntest.s -o test.o',
    ('test.o', 'test.s'): 'arm-linux-gnueabihf-objdump -D\ntest.o > test.s'
}
if __name__ == '__main__':
    graph = nx.DiGraph(edges.keys())
    figure(figsize=(16, 32), dpi=80)
    plt_instance = InteractiveGraph(
        graph,
        edge_labels=edges, edge_label_rotate=False, edge_layout='arc', arrows=True,
        edge_label_fontdict=dict(size=8, bbox=dict(boxstyle='round')),
        node_labels=True, node_size=2.5, edge_width=1, node_label_fontdict=dict(size=8),
    )
    plt.show()
