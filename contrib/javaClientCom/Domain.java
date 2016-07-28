package org.yars.communication;

public class Domain
{
  public double min = 0;
  public double max = 0;

  /** \brief Maps a value in the unit domain to a value in this domain. */
  public double map(double value)
  {
    return (max - min) * value + min;
  }

  /** \brief Maps a value into the unit domain. */
  public double remap(double value)
  {
    return (value - min) / (max - min);
  }
}
