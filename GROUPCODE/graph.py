import matplotlib.pyplot as plt 

def display_graph(amplitudes):
    x = list(range(len(amplitudes)))
    plt.plot(x,amplitudes)
    plt.show()
    print("Showing Graph...")

if __name__ == "__main__":
    x = list(range(10))
    display(x, [v*v for v in x])