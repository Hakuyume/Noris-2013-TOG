#include "pixel.hpp"

using namespace pixel;

Pixel::Pixel()
{
}

Pixel::Pixel(const Vector &pos, const Vector &moving)
    : p{pos}, m{moving}, active{true}
{
}

Vector Pixel::pos() const
{
  return p;
}

bool Pixel::isActive() const
{
  return active;
}

void Pixel::move()
{
  if (active)
    p += m;
}

void Pixel::check(const PixelSet &pixelSet)
{
  if (not active)
    return;

  for (const auto &q : pixelSet.findNeighborPixels(p, 1))
    if (m.dot(q.m) < 0) {
      active = false;
      return;
    }
}

PixelSet::PixelSet(const std::vector<Pixel> &pixels)
    : pixels{pixels}
{
  updateInfo();
}

void PixelSet::updateInfo()
{
  actives = 0;
  pixelMap.clear();

  for (const auto &p : pixels) {
    if (p.isActive())
      actives++;

    const auto it = pixelMap.find(p.pos());
    if (it == pixelMap.end())
      pixelMap.insert({p.pos(), {p}});
    else
      it->second.push_back(p);
  }
}

std::list<Pixel> PixelSet::findNeighborPixels(const Vector &p, const double &r) const
{
  std::list<Pixel> neighbors;

  for (int dx = -ceil(r); dx <= ceil(r); dx++)
    for (int dy = -ceil(r); dy <= ceil(r); dy++) {
      const auto it = pixelMap.find(p + Vector(dx, dy));

      if (it == pixelMap.end())
        continue;

      for (const auto &n : it->second)
        if ((p - n.pos()).norm() < r)
          neighbors.push_back(n);
    }

  return neighbors;
}

void PixelSet::updatePixels()
{
  for (auto &p : pixels)
    p.check(*this);

  for (auto &p : pixels)
    p.move();

  updateInfo();
}

size_t PixelSet::countActivePixels() const
{
  return actives;
}

std::vector<Pixel> PixelSet::allPixels() const
{
  return pixels;
}
