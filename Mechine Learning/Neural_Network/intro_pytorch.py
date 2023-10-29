import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torchvision import datasets, transforms

# Feel free to import other packages, if needed.
# As long as they are supported by CSL machines.


def get_data_loader(training = True):
    custom_transform =transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.1307,), (0.3081,))
        ])
    train_set=datasets.FashionMNIST('./data',train= training, download=True,transform=custom_transform)
    loader = torch.utils.data.DataLoader(train_set, batch_size = 64)
    return loader

def build_model():
    model = nn.Sequential(
        nn.Flatten(),
        nn.Linear(28*28, 128),
        nn.ReLU(),
        nn.Linear(128, 64),
        nn.ReLU(),
        nn.Linear(64, 10),
        nn.ReLU())
    return model

def train_model(model, train_loader, criterion, T):
    opt = optim.SGD(model.parameters(), lr=0.001, momentum=0.9)
    model.train()
    for epoch in range(T):
        accurate = 0
        n = len(train_loader.dataset)
        for data in train_loader:
            inputs, labels = data
            opt.zero_grad()
            outputs = model(inputs)
            for i in range(len(outputs)):
                prediction = torch.argmax(outputs[i])
                if  prediction == labels[i]:
                    accurate += 1
            loss = criterion(outputs, labels)
            loss.backward()
            opt.step()
            # print statistics
        print("Train Epoch: {}  Accuracy: {}/{}({:.2f}%) Loss: {:.3f}".format(epoch,accurate,n, 100*accurate/n, loss.item()))
        
def evaluate_model(model, test_loader, criterion, show_loss = True):
    model.eval()
    n = len(test_loader.dataset)
    with torch.no_grad():
        accurate = 0
        for data in test_loader:
            inputs, labels = data
            outputs = model(inputs)
            for i in range(len(outputs)):
                prediction = torch.argmax(outputs[i])
                if prediction == labels[i]:
                    accurate += 1
            loss = criterion(outputs, labels)
        if show_loss:
            print("Average loss: {:.4f}".format(loss.item()))
            print("Accuracy: {:.2f}%".format(100*accurate/n))
        else:
            print("Accuracy: {:.2f}%".format(100*accurate/n))
        
def predict_label(model, test_images, index):
    class_names = ['T-shirt/top','Trouser','Pullover','Dress','Coat','Sandal','Shirt','Sneaker','Bag','Ankle Boot']
    model.eval()
    outputs = model(test_images[index])
    prob = F.softmax(outputs, dim=-1)
    print(prob)
    v1, i1 = torch.kthvalue(prob, 10)
    v2, i2 = torch.kthvalue(prob, 9)
    v3, i3 = torch.kthvalue(prob, 8)
    print('{}: {:.2f}%'.format(class_names[i1], 100*v1.item()))
    print('{}: {:.2f}%'.format(class_names[i2], 100*v2.item()))
    print('{}: {:.2f}%'.format(class_names[i3], 100*v3.item()))

if __name__ == '__main__':
    #Q1
    train_loader = get_data_loader(True)
    test_loader = get_data_loader(False)
    print(type(train_loader))
    print(train_loader.dataset)
    print(test_loader.dataset)
   
    #Q2
    model = build_model()
    print(model)

    #Q3
    criterion = nn.CrossEntropyLoss()
    train_model(model, train_loader, criterion, 5)

    #Q4
    evaluate_model(model, test_loader, criterion, True)
    evaluate_model(model, test_loader, criterion, False)

    #Q5
    for data in test_loader:
        inputs, labels = data
        predict_label(model, inputs, 1)
        break
