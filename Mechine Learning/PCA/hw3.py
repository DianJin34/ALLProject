from ast import Lambda
from functools import total_ordering
from scipy.linalg import eigh
import numpy as np
import matplotlib.pyplot as plt

def load_and_center_dataset(filename):
    # Your implementation goes here!
    x = np.load(filename)
    return x - np.mean(x, axis=0)

def get_covariance(dataset):
    return np.dot(np.transpose(dataset), dataset)/(len(dataset - 1))

def get_eig(S, m):
    evalue, evector = eigh(S, subset_by_index=[len(S)-m, len(S) - 1])
    Lambda = np.diag(np.flipud(evalue))
    U = np.fliplr(evector)
    return Lambda, U

def get_eig_prop(S, prop):
    evalue, evector = eigh(S)
    total = sum(evalue)
    evalue, evector = eigh(S, subset_by_value=[total* prop, total])
    Lambda = np.diag(np.flipud(evalue))
    U = np.fliplr(evector)
    return Lambda, U

def project_image(image, U):
    tran = np.transpose(U)
    result = np.zeros(len(U))
    for i in range(len(U[0])):
        result += np.multiply(np.dot(tran[i], image), tran[i])
    return result

def display_image(orig, proj):
    result = proj.reshape(32, 32)
    result = np.transpose(result)
    orig = orig.reshape(32, 32)
    orig = np.transpose(orig)
    fig, (ax1, ax2) = plt.subplots(figsize=(12, 2), ncols=2)
    img1 = ax1.imshow(orig, aspect='equal')
    fig.colorbar(img1, ax = ax1)
    ax1.title.set_text("Original")

    img2 = ax2.imshow(result, aspect='equal')
    fig.colorbar(img2, ax = ax2)
    ax2.title.set_text("Projection")
    plt.show()
    pass