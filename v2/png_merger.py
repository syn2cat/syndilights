#!/usr/bon/env python

from PIL import Image

import redis
import datetime
import time


def merge(r):
    if not r.exists('new'):
        return None
    now = datetime.datetime.now().isoformat()
    r.rename('new', now)
    new_imgs = r.smembers(now)
    print('got new images', len(new_imgs))
    r.delete(now)
    if len(new_imgs) == 1:
        image = Image.frombytes('RGBA', (100, 100), list(new_imgs)[0])
        image.save(now + '.png', 'png')
    else:
        images = []
        for i in new_imgs:
            try:
                images.append(Image.frombytes('RGBA', (100, 100), i))
            except:
                continue
            if len(images) == 2:
                img_alpha = Image.alpha_composite(*images)
                images = [img_alpha]
        img_alpha.save(now + '.png', 'png')


if __name__ == "__main__":
    r = redis.Redis()
    while True:
        merge(r)
        time.sleep(10)
