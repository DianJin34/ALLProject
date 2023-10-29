import csv
import sys
import numpy as np
import matplotlib.pyplot as plt

def find_X_Y():
    with open(sys.argv[1]) as csv_file:
        csv_reader = list(csv.reader(csv_file))[1:]
        n = len(csv_reader)
        feature = np.ones((n, 2), dtype=np.int64)
        feature_for_graph = np.array([1]*n, dtype=np.int64)
        label = np.zeros(n, dtype=np.int64)
        for i in range(n):
            feature_for_graph[i] = csv_reader[i][0]
            feature[i][1] = csv_reader[i][0]
            label[i] = csv_reader[i][1]
        return feature, label, feature_for_graph
def plot_graph(Y, X_graph):
    fig = plt.figure()
    plt.plot(X_graph, Y)
    plt.show()
X, Y, X_graph = find_X_Y()
print("Q3a:")
print(X)
print("Q3b:")
print(Y)
Z = np.dot(np.transpose(X), X)
print("Q3c:")
print(Z)
I = np.linalg.inv(Z)
print("Q3d:")
print(I)
PI = np.dot(I, np.transpose(X))
print("Q3e:")
print(PI)
hat_beta = np.dot(PI, Y)
print("Q3f:")
print(hat_beta)
y_test = hat_beta[0]+ hat_beta[1]* 2021
print("Q4: " + str(y_test))
print("Q5a: " + "<")
print("Q5b: the year(Y) is higher, the open day(X) is lower, which means it will more likly unfreeze")
x_result = -hat_beta[0]/hat_beta[1]
print("Q6a: " + str(x_result))
print("Q6b: Yes, overall trend is decrease, and since we have trained around 200 datas, and the result is after around 700 years, so it should make sense")
plot_graph(Y, X_graph)