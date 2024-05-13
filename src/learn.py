import json

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader
import matplotlib.pyplot as plt

BATCH_SIZE = 64

def loadDataset(path: str):
    with open(path, 'r') as f:
        return json.load(f)

class puzzleDataset(Dataset):
    def __init__(self, dataset_path: str, data_range_begin: int, data_range_end: int):
        self.data_num = (data_range_end - data_range_begin) + 1
        self.data_path = dataset_path
        self.data_range_begin = data_range_begin
        self.data_range_end = data_range_end
        
        data = loadDataset(dataset_path)
        self.data = []
        for i in range(data_range_begin, data_range_end + 1):
            x1 = data[i]['input']['start']
            x2 = data[i]['input']['goal']
            y = data[i]['output']['n']
            self.data.append((x1, x2, y))

    def __len__(self):
        return self.data_num

    def __getitem__(self, idx: int):
        x1 = torch.Tensor(self.data[idx][0])
        x2 = torch.Tensor(self.data[idx][1])
        x = torch.cat([x1, x2], dim=0)
        y = torch.Tensor([self.data[idx][2]])

        return x, y

class PuzzleModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.fc1 = nn.Linear(32 * 32 * 2, 1024)
        self.fc2 = nn.Linear(1024, 512)
        self.fc3 =  nn.Linear(512, 256)
        self.fc4 = nn.Linear(256, 1)

    def forward(self, x: torch.Tensor):
        x = x.view(-1, 32 * 32 * 2)
        x = self.fc1(x)
        x = F.relu(x)
        x = self.fc2(x)
        x = F.relu(x)
        x = self.fc3(x)
        x = F.relu(x)
        x = self.fc4(x)
        return x


from tqdm import tqdm

dataset_size = 10000
train_rate = 0.8
valid_rate = 0.1
test_rate = 0.1

train_begin = 0
train_end = int(dataset_size * train_rate)
valid_begin = train_end + 1
valid_end = int(dataset_size * (train_rate + valid_rate))
test_begin = valid_end + 1
test_end = dataset_size - 1

print(f'train_begin: {train_begin}, train_end: {train_end}')
print(f'valid_begin: {valid_begin}, valid_end: {valid_end}')
print(f'test_begin: {test_begin}, test_end: {test_end}')

print('log: dataset loading...')

train_dataset = puzzleDataset('dataset/dataset.json', train_begin, train_end)
valid_dataset = puzzleDataset('dataset/dataset.json', valid_begin, valid_end)
test_dataset = puzzleDataset('dataset/dataset.json', test_begin, test_end)
train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True)
valid_loader = DataLoader(valid_dataset, batch_size=BATCH_SIZE)
test_loader = DataLoader(test_dataset, batch_size=BATCH_SIZE)

print('log: dataset loaded.')
print('log: model setup...')

model = PuzzleModel()

criterion = nn.MSELoss()

optimizer = optim.Adam(model.parameters(), lr=0.005)

print('log: model setup finished.')
print('log: learning...')

epochs = 100

train_losses = []
valid_losses = []

if torch.cuda.is_available():
    device = 'cuda'
    print('log: training on GPU...')
else:
    device = 'cpu'
    print('log: training on CPU...')

# training
for epoch in range(epochs):
    train_loss = 0
    model.train()
    for inputs, labels in tqdm(train_loader):
        inputs, labels = inputs.to(device), labels.to(device)
        optimizer.zero_grad()
        outputs = model(inputs)
        if torch.cuda.is_available():
            outputs = outputs.cpu()
        loss = criterion(outputs.view(-1, 1), labels.view(-1, 1))
        loss.backward()
        optimizer.step()
        train_loss += loss.item()
    train_losses.append(train_loss / len(train_loader))
    print(f'Epoch: {epoch + 1}, Loss: {train_loss / len(train_loader)}')

    # validation
    valid_loss = 0
    model.eval()
    with torch.no_grad():
        for inputs, labels in tqdm(valid_loader):
            inputs, labels = inputs.to(device), labels.to(device)
            outputs = model(inputs)
            loss = criterion(outputs.view(-1, 1), labels.view(-1, 1))
            valid_loss += loss.item()
    valid_losses.append(valid_loss / len(valid_loader))
    print(f'Epoch: {epoch + 1}, Validation Loss: {valid_loss / len(valid_loader)}')

print('log: learning finished.')

# plot loss
plt.plot(range(epochs), train_losses, label='train_loss')
plt.plot(range(epochs), valid_losses, label='valid_loss')
plt.xlabel('epochs')
plt.ylabel('loss')
plt.legend()
plt.savefig('models/loss.png')

# evaluation
correct = 0
total = 0
model.eval()
with torch.no_grad():
    for inputs, labels in tqdm(test_loader):
        inputs, labels = inputs.to(device), labels.to(device)
        outputs = model(inputs)
        predicted = torch.round(outputs.data)
        total += labels.size(0)
        correct += (predicted == labels).sum().item()

print(f'Accuracy: {100 * correct / total}%')

# save model
torch.save(model.state_dict(), 'models/model.pth')
