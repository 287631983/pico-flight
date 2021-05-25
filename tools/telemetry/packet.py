import queue

class Sender:
  def __init__(self, handlers:dict) -> None:
    self.queue = queue.Queue()
    self.handlers = handlers
    
  def send(self, type, payload):
    if not type in self.handlers:
      return
    
    self.queue.put((type,self.handlers[type].build(payload)))
  
  def serialize(self, type, payload):
    pass
  
  def update(self, writer):
    while not self.queue.empty():
      self.serialize(writer, self.queue.get())
      

      
class Receiver:
  def __init__(self, handlers:dict) -> None:
      self.handlers = handlers
    
  def next(self, byte):
    pass    
        
  def processPayload(self, payload):
    if self.type in self.handlers:  
      handler = self.handlers[self.type]
      if handler:
        handler(payload)        

    
class Reader:
  def __init__(self) -> None:
    pass
  
  def update(self):
    pass


class Writer:  
  def write(self, data):
    pass
    

class Packetizer:
  def __init__(self, reader:Reader, writer:Writer, receiver:Receiver, sender:Sender) -> None:
      self.reader = reader
      self.writer = writer
      self.receiver = receiver
      self.sender = sender

  def update(self):
    self.reader.update()
    
    for byte in self.reader.available():
      self.receiver.next(byte)
      
    self.sender.update(self.writer)
      
      
    
      
      
        
    