# from ast import MatchSequence
from csv import DictReader
from turtle import distance
from matplotlib import pyplot as plt
import numpy as np
import sys
from scipy.cluster import hierarchy
def load_data(filepath):
    result = list()
    with open(filepath, 'r') as read_obj:
        csv_dict_reader = DictReader(read_obj)
        for row in csv_dict_reader:
            result.append(row)
    return result

def calc_features(row):
    result = np.zeros((6,), dtype = np.int64)
    result[0] = row["Attack"]
    result[1] = row["Sp. Atk"]
    result[2] = row["Speed"]
    result[3] = row["Defense"]
    result[4] = row["Sp. Def"]
    result[5] = row["HP"]
    return result

def hac(features):
    n = len(features)
    #Create Matrix can store all Pokimon
    Matrix = np.zeros((n,n))
    result = np.zeros((n-1,4))
    index = -1
    for i in range(0, n):
        for j in range(0, n):
            if i == j:
                #Make the Pokimon have infinte length to itself
                Matrix[i][j] = sys.maxsize
            else:
                #caculate the distance between every Pokimon
                Matrix[i][j] = np.linalg.norm(np.array(features[i]) - np.array(features[j]))


    cluster_index = np.array([index]*(n))
    for i in range(0, n-1):
            #check the min distance
            Distance_min = np.amin(Matrix)
            index_Small_min = sys.maxsize
            index_Big_min = sys.maxsize
            for row_j in range(n):
                for column_k in range(n):
                    if Matrix[row_j][column_k] == Distance_min:
                        if cluster_index[row_j] == -1:
                            answer1 = row_j
                        else:
                            answer1 = cluster_index[row_j]+n
                        if cluster_index[column_k] == -1:
                            answer2 = column_k
                        else:
                            answer2 = cluster_index[column_k]+n
                        if answer1 > answer2:
                            answer1_big = True
                        else:
                            answer1_big = False
                        index_Small = min(answer1, answer2)
                        index_Big = max(answer1, answer2)
                        #update smallest index
                        if index_Small < index_Small_min:
                            index_Small_min = index_Small
                            index_Big_min = index_Big
                            if answer1_big:
                                 row = row_j
                                 column = column_k
                            else:
                                row = column_k
                                column = row_j
                        #if small index is same but big index is smaller than before
                        elif index_Small == index_Big_min and index_Big < index_Big_min:
                            index_Small_min = index_Small
                            index_Big_min = index_Big
                            if answer1_big:
                                row = row_j
                                column = column_k
                            else:
                                row = column_k
                                column = row_j

            cluster_for_row = list()
            cluster_for_column = list()
            #when only one value in cluster
            if cluster_index[row] == -1:
                cluster_for_row.append(row)
            #when there are many point in cluster
            else:
                cluster_for_row = [j for j in range(len(cluster_index)) if cluster_index[j] == cluster_index[row]]
            if cluster_index[column] == -1:
                cluster_for_column.append(column)
            else:
                cluster_for_column = [j for j in range(len(cluster_index)) if cluster_index[j] == cluster_index[column]]
            
            #update the return value
            cluster_merge = cluster_for_column + cluster_for_row
            result[i][0] = index_Small_min
            result[i][1] = index_Big_min
            result[i][2] = Distance_min
            result[i][3] = len(cluster_merge)

            #update the Matrix value (distance)
            for col in cluster_for_column:
                for r in cluster_for_row:
                    Matrix[r][col] = sys.maxsize
            for j in range(0,n):
                # update the cluster to farest distance from the other cluster in Matrix
                if j not in (cluster_merge):
                    Matrix[j][cluster_merge] = max(Matrix[j][k] for k in cluster_merge)
            for j in cluster_merge:
                for z in range(0, n):
                    Matrix[j][z] = Matrix[z][j]
            #update in cluster data
            cluster_index[cluster_merge] = i           
    return result

def imshow_hac(Z):
    fig = plt.figure(figsize = (25, 10))
    dn = hierarchy.dendrogram(Z)
    plt.show()

def main():
    data = hac([calc_features(row) for row in load_data('Pokemon.csv')][:10])
    imshow_hac(data)
    
