/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Magister Solutions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Budiarto Herman <budiarto.herman@magister.fi>
 *
 */

#ifndef HISTOGRAM_PLOT_HELPER_H
#define HISTOGRAM_PLOT_HELPER_H

#include <ns3/callback.h>
#include <fstream>
#include <cmath>


namespace ns3 {


/**
 * \defgroup traffic Traffic Generators
 * \brief Collection of applications which model realistic network traffic.
 */


/**
 * \ingroup traffic
 * \brief Class with a static method to generate a histogram (as a Gnuplot file)
 *        from a specified random variable.
 *
 * For usage example, see http-variables-plot.cc and nrtv-variables-plot.cc in
 * `src/traffic/examples/`.
 *
 * \sa Plot()
 */
class HistogramPlotHelper
{
public:
  /**
   * \brief Write a Gnuplot file of a histogram from a given random variable.
   *
   * \param valueStream a callback to the function that returns a random value
   *                    of type `T` (must be specified as the function's
   *                    template argument)
   * \param name the name of the plot, which determines the output file name
   * \param plotTitle the text to be printed on top of the histogram
   * \param axisLabel the text to be printed as the label of the histogram's
   *                  X axis
   * \param numOfSamples determines how many samples are retrieved from the
   *                     function specified in `valueStream` argument; higher
   *                     values produces smoother curve but requires more
   *                     processing time
   * \param binWidth the width of each histogram bar (in the same unit as the
   *                 return values of the function specified in `valueStream`
   *                 argument)
   * \param referenceMean a mean value (in the same unit as the return values of
   *                      the function specified in `valueStream` argument) to
   *                      be printed on the histogram for comparison purpose
   * \param max an optional argument that can be specified to determine the
   *            maximum value of the random values to be considered in the
   *            histogram; if unspecified, the histogram will automatically
   *            compute the maximum value in proportion to the `referenceMean`
   *            argument
   *
   * For usage example, see `http-variables-plot.cc` and `nrtv-variables-plot.cc`
   * in `src/traffic/examples/`.
   *
   * Taking for example "histogram" as the value of the parameter `name`, then
   * this method will generate a Gnuplot file in the current working directory
   * with the name "histogram.plt". This file can be further converted to an
   * image file (named "histogram.png") using the following command:
   *
   *     $ gnuplot histogram.plt
   *
   * The method will print the sentence "Output file written: histogram.plt" to
   * the standard output when it completes successfully.
   *
   * The generated histogram is the graphical representation of the distribution
   * of random values. The random values are grouped (i.e., tabulated) into
   * discrete intervals called "bins", which are represented in the histogram as
   * vertical bars. The height of the bar is the frequency of observations in
   * the interval over all the retrieved random value samples.
   *
   * The function also computes the mean of all the retrieved samples and print
   * it on the histogram as the "actual mean". In addition, a "reference mean",
   * which is provided as an argument, is also printed on the histogram for
   * comparison purpose.
   */
  template<typename T> static void
  Plot (Callback<T> valueStream, std::string name,
        std::string plotTitle, std::string axisLabel,
        uint32_t numOfSamples, T binWidth,
        double referenceMean, T max = 0);
};


/*
 * The following method is defined here in .h file, because static templated
 * function like this is not visible to the linker if put in .cc file.
 */

template<typename T> void
HistogramPlotHelper::Plot (Callback<T> valueStream, std::string name,
                           std::string plotTitle, std::string axisLabel,
                           uint32_t numOfSamples, T binWidth,
                           double referenceMean, T max)
{
  std::string plotFileName = name + ".plt";
  std::ofstream ofs (plotFileName.c_str ());

  if (!ofs.is_open ())
    {
      NS_FATAL_ERROR ("Unable to write to " << plotFileName);
    }

  ofs << "set terminal png" << std::endl;
  ofs << "set output '" << name << ".png'" << std::endl;

  ofs << "set title '" << plotTitle << "'" << std::endl;
  ofs << "set xlabel '" << axisLabel << "'" << std::endl;
  ofs << "set ylabel 'Frequency (out of " << numOfSamples << " samples)'"
      << std::endl;

  if (static_cast<uint32_t> (max) == 0)
    {
      /*
       * Maximum value is not specified as input argument, so we compute it
       * "automatically" here. Nothing really special in the formula, just a
       * value that produces rather good-looking results.
       */
      ofs << "set xrange [0:" << 2 * exp (1) * referenceMean << "]"
          << std::endl;
    }
  else
    {
      // add 10% offset on top of the specified maximum value
      ofs << "set xrange [0:" << 1.1 * max << "]" << std::endl;
    }

  // ignoring negative values (if any)
  ofs << "set yrange [0:]" << std::endl;
  // so that tics don't step on the histogram
  ofs << "set tics out nomirror" << std::endl;
// the width of each bar
  ofs << "set boxwidth " << binWidth << std::endl;
  // the function to determine which bin a sample belongs to
  ofs << "bin(x)=" << binWidth << "*floor(x/" << binWidth << ")"
      << "+" << (0.5 * binWidth) << std::endl;
  // definition of the histogram plot
  ofs << "plot '-' using (bin($1)):(1.0/" << numOfSamples << ") "
      << "smooth freq with boxes notitle, "
      << "'-' title 'Reference mean' with points, "
      << "'-' title 'Actual mean' with points" << std::endl;

  // start writing the data points for the histogram
  T value;
  T sum = 0;
  for (uint32_t i = 0; i < numOfSamples; i++)
    {
      value = valueStream ();
      sum += value;
      ofs << valueStream () << std::endl;
    }
  ofs << "e" << std::endl; // separator between series

  // write the reference mean data point
  ofs << referenceMean << " 0" << std::endl;
  ofs << "e" << std::endl; // separator between series

  // write the actual mean data point
  ofs << (sum / static_cast<double> (numOfSamples)) << " 0" << std::endl;
  ofs << "e" << std::endl; // separator between series

  ofs.close ();

  std::cout << "Output file written: " << plotFileName << std::endl;

} // end of `void Plot (...)`


} // end of `namespace ns3`


#endif /* HISTOGRAM_PLOT_HELPER_H */
