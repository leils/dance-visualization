import c3d

with open('data.c3d', 'rb') as handle:
  reader = c3d.Reader(handle);
  for i, (points, analog) in enumerate(reader.read_frames()):
    print('Frame {}: {}'.format(i, points.round(2)))
